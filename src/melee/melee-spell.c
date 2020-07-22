﻿#include "melee/melee-spell.h"
#include "core/disturbance.h"
#include "core/player-redraw-types.h"
#include "dungeon/dungeon-flag-types.h"
#include "dungeon/dungeon.h"
#include "dungeon/quest.h"
#include "effect/effect-characteristics.h"
#include "floor/cave.h"
#include "floor/floor.h"
#include "melee/melee-spell-flags-checker.h"
#include "melee/melee-spell-util.h"
#include "monster-floor/monster-move.h"
#include "monster-race/monster-race.h"
#include "monster-race/race-flags-ability1.h"
#include "monster-race/race-flags-ability2.h"
#include "monster-race/race-flags2.h"
#include "monster-race/race-flags3.h"
#include "monster-race/race-flags4.h"
#include "monster-race/race-flags7.h"
#include "monster-race/race-indice-types.h"
#include "monster/monster-describer.h"
#include "monster/monster-info.h"
#include "monster/monster-status.h"
#include "mspell/assign-monster-spell.h"
#include "mspell/mspell-judgement.h"
#include "mspell/mspell-mask-definitions.h"
#include "mspell/mspell-util.h"
#include "mspell/mspells1.h"
#include "pet/pet-util.h"
#include "spell-kind/spells-world.h"
#include "spell-realm/spells-hex.h"
#include "spell/spell-types.h"
#include "system/floor-type-definition.h"
#include "system/monster-type-definition.h"
#include "view/display-messages.h"
#include "world/world.h"
#ifdef JP
#else
#include "monster/monster-description-types.h"
#endif

#define RF4_SPELL_SIZE 32
#define RF5_SPELL_SIZE 32
#define RF6_SPELL_SIZE 32

static bool try_melee_spell(player_type *target_ptr, melee_spell_type *ms_ptr)
{
    if (spell_is_inate(ms_ptr->thrown_spell) || (!ms_ptr->in_no_magic_dungeon && (!monster_stunned_remaining(ms_ptr->m_ptr) || one_in_(2))))
        return FALSE;

    disturb(target_ptr, TRUE, TRUE);
    if (ms_ptr->see_m)
        msg_format(_("%^sは呪文を唱えようとしたが失敗した。", "%^s tries to cast a spell, but fails."), ms_ptr->m_name);

    return TRUE;
}

static bool disturb_melee_spell(player_type *target_ptr, melee_spell_type *ms_ptr)
{
    if (spell_is_inate(ms_ptr->thrown_spell) || !magic_barrier(target_ptr, ms_ptr->m_idx))
        return FALSE;

    if (ms_ptr->see_m)
        msg_format(_("反魔法バリアが%^sの呪文をかき消した。", "Anti magic barrier cancels the spell which %^s casts."), ms_ptr->m_name);

    return TRUE;
}

/*!
 * @brief モンスターが敵モンスターに特殊能力を使う処理のメインルーチン /
 * Monster tries to 'cast a spell' (or breath, etc) at another monster.
 * @param target_ptr プレーヤーへの参照ポインタ
 * @param m_idx 術者のモンスターID
 * @return 実際に特殊能力を使った場合TRUEを返す
 * @details
 * The player is only disturbed if able to be affected by the spell.
 */
bool monst_spell_monst(player_type *target_ptr, MONSTER_IDX m_idx)
{
    melee_spell_type tmp_ms;
    melee_spell_type *ms_ptr = initialize_melee_spell_type(target_ptr, &tmp_ms, m_idx);
    if (!check_melee_spell_set(target_ptr, ms_ptr))
        return FALSE;

    /* Get the monster name (or "it") */
    monster_desc(target_ptr, ms_ptr->m_name, ms_ptr->m_ptr, 0x00);
#ifdef JP
#else
    /* Get the monster possessive ("his"/"her"/"its") */
    monster_desc(target_ptr, ms_ptr->m_poss, ms_ptr->m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

    /* Get the target's name (or "it") */
    GAME_TEXT t_name[160];
    monster_desc(target_ptr, t_name, ms_ptr->t_ptr, 0x00);
    ms_ptr->thrown_spell = ms_ptr->spell[randint0(ms_ptr->num)];
    if (target_ptr->riding && (m_idx == target_ptr->riding))
        disturb(target_ptr, TRUE, TRUE);

    if (try_melee_spell(target_ptr, ms_ptr) || disturb_melee_spell(target_ptr, ms_ptr))
        return TRUE;

    ms_ptr->can_remember = is_original_ap_and_seen(target_ptr, ms_ptr->m_ptr);
    ms_ptr->dam = monspell_to_monster(target_ptr, ms_ptr->thrown_spell, ms_ptr->y, ms_ptr->x, m_idx, ms_ptr->target_idx, FALSE);
    if (ms_ptr->dam < 0)
        return FALSE;

    bool is_special_magic = ms_ptr->m_ptr->ml;
    is_special_magic &= ms_ptr->maneable;
    is_special_magic &= current_world_ptr->timewalk_m_idx == 0;
    is_special_magic &= !target_ptr->blind;
    is_special_magic &= target_ptr->pclass == CLASS_IMITATOR;
    is_special_magic &= ms_ptr->thrown_spell != 167; /* Not RF6_SPECIAL */
    if (is_special_magic) {
        if (target_ptr->mane_num == MAX_MANE) {
            target_ptr->mane_num--;
            for (int i = 0; i < target_ptr->mane_num - 1; i++) {
                target_ptr->mane_spell[i] = target_ptr->mane_spell[i + 1];
                target_ptr->mane_dam[i] = target_ptr->mane_dam[i + 1];
            }
        }

        target_ptr->mane_spell[target_ptr->mane_num] = ms_ptr->thrown_spell - RF4_SPELL_START;
        target_ptr->mane_dam[target_ptr->mane_num] = ms_ptr->dam;
        target_ptr->mane_num++;
        target_ptr->new_mane = TRUE;

        target_ptr->redraw |= PR_IMITATION;
    }

    if (ms_ptr->can_remember) {
        if (ms_ptr->thrown_spell < RF4_SPELL_START + RF4_SPELL_SIZE) {
            ms_ptr->r_ptr->r_flags4 |= (1L << (ms_ptr->thrown_spell - RF4_SPELL_START));
            if (ms_ptr->r_ptr->r_cast_spell < MAX_UCHAR)
                ms_ptr->r_ptr->r_cast_spell++;
        } else if (ms_ptr->thrown_spell < RF5_SPELL_START + RF5_SPELL_SIZE) {
            ms_ptr->r_ptr->r_flags5 |= (1L << (ms_ptr->thrown_spell - RF5_SPELL_START));
            if (ms_ptr->r_ptr->r_cast_spell < MAX_UCHAR)
                ms_ptr->r_ptr->r_cast_spell++;
        } else if (ms_ptr->thrown_spell < RF6_SPELL_START + RF6_SPELL_SIZE) {
            ms_ptr->r_ptr->r_flags6 |= (1L << (ms_ptr->thrown_spell - RF6_SPELL_START));
            if (ms_ptr->r_ptr->r_cast_spell < MAX_UCHAR)
                ms_ptr->r_ptr->r_cast_spell++;
        }
    }

    if (target_ptr->is_dead && (ms_ptr->r_ptr->r_deaths < MAX_SHORT) && !target_ptr->current_floor_ptr->inside_arena)
        ms_ptr->r_ptr->r_deaths++;

    return TRUE;
}
