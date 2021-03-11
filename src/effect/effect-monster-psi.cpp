﻿#include "effect/effect-monster-psi.h"
#include "core/player-redraw-types.h"
#include "core/window-redrawer.h"
#include "effect/effect-monster-util.h"
#include "floor/line-of-sight.h"
#include "mind/mind-mirror-master.h"
#include "monster-race/monster-race.h"
#include "monster-race/race-flags1.h"
#include "monster-race/race-flags2.h"
#include "monster-race/race-flags3.h"
#include "monster/monster-describer.h"
#include "monster/monster-description-types.h"
#include "monster/monster-info.h"
#include "player/player-damage.h"
#include "status/bad-status-setter.h"
#include "util/bit-flags-calculator.h"
#include "view/display-messages.h"
#include "world/world.h"

/*!
 * @brief 精神のないモンスターのPsi攻撃に対する完全な耐性を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return 完全な耐性を発動した場合TRUE、そうでなければFALSE
 */
static bool effect_monster_psi_empty_mind(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (none_bits(em_ptr->r_ptr->flags2, RF2_EMPTY_MIND))
        return FALSE;

    em_ptr->dam = 0;
    em_ptr->note = _("には完全な耐性がある！", " is immune.");
    if (is_original_ap_and_seen(caster_ptr, em_ptr->m_ptr))
        set_bits(em_ptr->r_ptr->r_flags2, RF2_EMPTY_MIND);

    return TRUE;
}

/*!
 * @brief 異質な精神のモンスター及び強力なモンスターのPsi攻撃に対する耐性を発動する
 * @param em_ptr モンスター効果への参照ポインタ
 * @return 耐性を発動した場合TRUE、そうでなければFALSE
 * @detail
 * 以下のいずれかの場合は耐性がある
 * 1) STUPIDまたはWIERD_MINDである
 * 2) ANIMALである
 * 3) レベルが d(3*ダメージ) より大きい
 */
static bool effect_monster_psi_weird_mind_and_powerful(effect_monster_type *em_ptr)
{
    bool has_resistance = any_bits(em_ptr->r_ptr->flags2, RF2_STUPID | RF2_WEIRD_MIND) || any_bits(em_ptr->r_ptr->flags3, RF3_ANIMAL)
        || (em_ptr->r_ptr->level > randint1(3 * em_ptr->dam));
    if (!has_resistance)
        return FALSE;

    em_ptr->note = _("には耐性がある！", " resists!");
    em_ptr->dam /= 3;
    return TRUE;
}

/*!
 * @brief 堕落した精神のモンスターへのPsi攻撃のダメージ反射を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return ダメージ反射を発動した場合TRUE、そうでなければFALSE
 * @detail
 * 以下の条件を満たす場合に 1/2 の確率でダメージ反射する
 * 1) UNDEADまたはDEMONである
 * 2) レベルが詠唱者の レベル/2 より大きい
 */
static bool effect_monster_psi_corrupted(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    bool is_corrupted = any_bits(em_ptr->r_ptr->flags3, RF3_UNDEAD | RF3_DEMON) && (em_ptr->r_ptr->level > caster_ptr->lev / 2) && one_in_(2);
    if (!is_corrupted)
        return FALSE;

    em_ptr->note = NULL;
    msg_format(_("%^sの堕落した精神は攻撃を跳ね返した！",
                   (em_ptr->seen ? "%^s's corrupted mind backlashes your attack!" : "%^ss corrupted mind backlashes your attack!")),
        em_ptr->m_name);
    return TRUE;
}

/*!
 * @brief モンスターがPsi攻撃をダメージ反射した場合のプレイヤーへの追加効果を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return なし
 * @detail
 * 効果は、混乱、朦朧、恐怖、麻痺
 * 3/4の確率または影分身時はダメージ及び追加効果はない。
 */
static void effect_monster_psi_reflect_extra_effect(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (!one_in_(4) || check_multishadow(caster_ptr))
        return;

    switch (randint1(4)) {
    case 1:
        set_confused(caster_ptr, caster_ptr->confused + 3 + randint1(em_ptr->dam));
        break;
    case 2:
        set_stun(caster_ptr, caster_ptr->stun + randint1(em_ptr->dam));
        break;
    case 3: {
        if (any_bits(em_ptr->r_ptr->flags3, RF3_NO_FEAR))
            em_ptr->note = _("には効果がなかった。", " is unaffected.");
        else
            set_afraid(caster_ptr, caster_ptr->afraid + 3 + randint1(em_ptr->dam));

        break;
    }
    default:
        if (!caster_ptr->free_act)
            (void)set_paralyzed(caster_ptr, caster_ptr->paralyzed + randint1(em_ptr->dam));

        break;
    }
}

/*!
 * @brief モンスターのPsi攻撃に対する耐性を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return なし
 * @detail
 * 耐性を発動した精神の堕落したモンスターは効力を跳ね返すことがある。
 */
static void effect_monster_psi_resist(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (effect_monster_psi_empty_mind(caster_ptr, em_ptr))
        return;
    if (effect_monster_psi_weird_mind_and_powerful(em_ptr))
        return;
    if (!effect_monster_psi_corrupted(caster_ptr, em_ptr))
        return;

    /* プレイヤーの反射判定 */
    if ((randint0(100 + em_ptr->r_ptr->level / 2) < caster_ptr->skill_sav) && !check_multishadow(caster_ptr)) {
        msg_print(_("しかし効力を跳ね返した！", "You resist the effects!"));
        em_ptr->dam = 0;
        return;
    }

    /* Injure +/- confusion */
    monster_desc(caster_ptr, em_ptr->killer, em_ptr->m_ptr, MD_WRONGDOER_NAME);
    take_hit(caster_ptr, DAMAGE_ATTACK, em_ptr->dam, em_ptr->killer, -1);
    effect_monster_psi_reflect_extra_effect(caster_ptr, em_ptr);
    em_ptr->dam = 0;
}

/*!
 * @brief モンスターへのPsi攻撃の追加効果を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return なし
 * @detail
 * 効果は、混乱、朦朧、恐怖、麻痺(各耐性無効)
 * ダメージがないか3/4の確率で効果なし
 */
static void effect_monster_psi_extra_effect(effect_monster_type *em_ptr)
{
    if ((em_ptr->dam <= 0) || !one_in_(4))
        return;

    switch (randint1(4)) {
    case 1:
        em_ptr->do_conf = 3 + randint1(em_ptr->dam);
        break;
    case 2:
        em_ptr->do_stun = 3 + randint1(em_ptr->dam);
        break;
    case 3:
        em_ptr->do_fear = 3 + randint1(em_ptr->dam);
        break;
    default:
        em_ptr->note = _("は眠り込んでしまった！", " falls asleep!");
        em_ptr->do_sleep = 3 + randint1(em_ptr->dam);
        break;
    }
}

/*!
 * @brief モンスターへのPsi攻撃(GF_PSI)の効果を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return PROICESS_CONTINUE
 * @detail
 * 視界による影響を発動する。
 * モンスターの耐性とそれに不随した効果を発動する。
 */
process_result effect_monster_psi(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (em_ptr->seen)
        em_ptr->obvious = TRUE;
    if (!(los(caster_ptr, em_ptr->m_ptr->fy, em_ptr->m_ptr->fx, caster_ptr->y, caster_ptr->x))) {
        if (em_ptr->seen_msg)
            msg_format(_("%sはあなたが見えないので影響されない！", "%^s can't see you, and isn't affected!"), em_ptr->m_name);

        em_ptr->skipped = TRUE;
        return PROCESS_CONTINUE;
    }

    effect_monster_psi_resist(caster_ptr, em_ptr);
    effect_monster_psi_extra_effect(em_ptr);
    em_ptr->note_dies = _("の精神は崩壊し、肉体は抜け殻となった。", " collapses, a mindless husk.");
    return PROCESS_CONTINUE;
}

/*!
 * @brief モンスターのPsi攻撃(GF_PSI_DRAIN)に対する耐性を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return なし
 * @detail
 * 耐性を発動した精神の堕落したモンスターは効力を跳ね返すことがある。
 */
static void effect_monster_psi_drain_resist(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (effect_monster_psi_corrupted(caster_ptr, em_ptr))
        return;

    /* プレイヤーの反射判定 */
    if ((randint0(100 + em_ptr->r_ptr->level / 2) < caster_ptr->skill_sav) && !check_multishadow(caster_ptr)) {
        msg_print(_("あなたは効力を跳ね返した！", "You resist the effects!"));
        em_ptr->dam = 0;
        return;
    }

    monster_desc(caster_ptr, em_ptr->killer, em_ptr->m_ptr, MD_WRONGDOER_NAME);
    if (check_multishadow(caster_ptr)) {
        take_hit(caster_ptr, DAMAGE_ATTACK, em_ptr->dam, em_ptr->killer, -1);
        em_ptr->dam = 0;
        return;
    }

    msg_print(_("超能力パワーを吸いとられた！", "Your psychic energy is drained!"));
    caster_ptr->csp -= damroll(5, em_ptr->dam) / 2;
    if (caster_ptr->csp < 0)
        caster_ptr->csp = 0;

    set_bits(caster_ptr->redraw, PR_MANA);
    set_bits(caster_ptr->window_flags, PW_SPELL);
    take_hit(caster_ptr, DAMAGE_ATTACK, em_ptr->dam, em_ptr->killer, -1);
    em_ptr->dam = 0;
}

/*!
 * @brief モンスターへのPsi攻撃(GF_PSI_DRAIN)のダメージをMPに変換する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return なし
 */
static void effect_monster_psi_drain_change_power(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    int b = damroll(5, em_ptr->dam) / 4;
    concptr str = (caster_ptr->pclass == CLASS_MINDCRAFTER) ? _("超能力パワー", "psychic energy") : _("魔力", "mana");
    concptr msg = _("あなたは%sの苦痛を%sに変換した！", (em_ptr->seen ? "You convert %s's pain into %s!" : "You convert %ss pain into %s!"));
    msg_format(msg, em_ptr->m_name, str);

    b = MIN(caster_ptr->msp, caster_ptr->csp + b);
    caster_ptr->csp = b;
    set_bits(caster_ptr->redraw, PR_MANA);
    set_bits(caster_ptr->window_flags, PW_SPELL);
}

/*!
 * @brief モンスターへのPsi攻撃(GF_PSI_DRAIN)の効果を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return PROICESS_CONTINUE
 * @detail
 * ダメージがないか3/4の確率で追加効果なし
 */
process_result effect_monster_psi_drain(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (em_ptr->seen)
        em_ptr->obvious = TRUE;

    if (em_ptr->r_ptr->flags2 & RF2_EMPTY_MIND) {
        em_ptr->dam = 0;
        em_ptr->note = _("には完全な耐性がある！", " is immune.");
    } else if ((em_ptr->r_ptr->flags2 & (RF2_STUPID | RF2_WEIRD_MIND)) || (em_ptr->r_ptr->flags3 & RF3_ANIMAL)
        || (em_ptr->r_ptr->level > randint1(3 * em_ptr->dam))) {
        effect_monster_psi_drain_resist(caster_ptr, em_ptr);
    } else if (em_ptr->dam > 0) {
        effect_monster_psi_drain_change_power(caster_ptr, em_ptr);
    }

    em_ptr->note_dies = _("の精神は崩壊し、肉体は抜け殻となった。", " collapses, a mindless husk.");
    return PROCESS_CONTINUE;
}

/*!
 * @brief モンスターへのテレキネシス(GF_TELEKINESIS)の効果を発動する
 * @param caster_ptr プレイヤーへの参照ポインタ
 * @param em_ptr モンスター効果への参照ポインタ
 * @return PROICESS_CONTINUE
 * @detail
 * 朦朧＋ショートテレポートアウェイ
 */
process_result effect_monster_telekinesis(player_type *caster_ptr, effect_monster_type *em_ptr)
{
    if (em_ptr->seen)
        em_ptr->obvious = TRUE;
    if (one_in_(4)) {
        if (caster_ptr->riding && (em_ptr->g_ptr->m_idx == caster_ptr->riding))
            em_ptr->do_dist = 0;
        else
            em_ptr->do_dist = 7;
    }

    em_ptr->do_stun = damroll((em_ptr->caster_lev / 20) + 3, em_ptr->dam) + 1;
    if (any_bits(em_ptr->r_ptr->flags1, RF1_UNIQUE) || (em_ptr->r_ptr->level > 5 + randint1(em_ptr->dam))) {
        em_ptr->do_stun = 0;
        em_ptr->obvious = FALSE;
    }

    return PROCESS_CONTINUE;
}
