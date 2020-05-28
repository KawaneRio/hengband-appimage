﻿#include "system/angband.h"
#include "market/bounty.h"
#include "world/world.h"
#include "player/avatar.h"
#include "market/bounty-prize-table.h"
#include "object/item-apply-magic.h"
#include "object/object-flavor.h"
#include "autopick/autopick.h"
#include "core/stuff-handler.h"
#include "term/gameterm.h"
#include "market/building-util.h"
#include "dungeon/dungeon.h"
#include "monster/monster-race-hook.h"

/*!
 * @brief 賞金首の引き換え処理 / Get prize
 * @param player_ptr プレーヤーへの参照ポインタ
 * @return 各種賞金首のいずれかでも換金が行われたか否か。
 */
bool exchange_cash(player_type *player_ptr)
{
    bool change = FALSE;
    GAME_TEXT o_name[MAX_NLEN];
    object_type *o_ptr;

    for (INVENTORY_IDX i = 0; i <= INVEN_LARM; i++) {
        o_ptr = &player_ptr->inventory_list[i];
        if ((o_ptr->tval == TV_CAPTURE) && (o_ptr->pval == MON_TSUCHINOKO)) {
            char buf[MAX_NLEN + 20];
            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%s を換金しますか？", "Convert %s into money? "), o_name);
            if (get_check(buf)) {
                msg_format(_("賞金 %ld＄を手に入れた。", "You get %ldgp."), (long int)(1000000L * o_ptr->number));
                player_ptr->au += 1000000L * o_ptr->number;
                player_ptr->redraw |= (PR_GOLD);
                vary_item(player_ptr, i, -o_ptr->number);
            }

            change = TRUE;
        }
    }

    for (INVENTORY_IDX i = 0; i < INVEN_PACK; i++) {
        o_ptr = &player_ptr->inventory_list[i];
        if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_TSUCHINOKO)) {
            char buf[MAX_NLEN + 20];
            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%s を換金しますか？", "Convert %s into money? "), o_name);
            if (get_check(buf)) {
                msg_format(_("賞金 %ld＄を手に入れた。", "You get %ldgp."), (long int)(200000L * o_ptr->number));
                player_ptr->au += 200000L * o_ptr->number;
                player_ptr->redraw |= (PR_GOLD);
                vary_item(player_ptr, i, -o_ptr->number);
            }

            change = TRUE;
        }
    }

    for (INVENTORY_IDX i = 0; i < INVEN_PACK; i++) {
        o_ptr = &player_ptr->inventory_list[i];
        if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_TSUCHINOKO)) {
            char buf[MAX_NLEN + 20];
            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%s を換金しますか？", "Convert %s into money? "), o_name);
            if (get_check(buf)) {
                msg_format(_("賞金 %ld＄を手に入れた。", "You get %ldgp."), (long int)(100000L * o_ptr->number));
                player_ptr->au += 100000L * o_ptr->number;
                player_ptr->redraw |= (PR_GOLD);
                vary_item(player_ptr, i, -o_ptr->number);
            }

            change = TRUE;
        }
    }

    for (INVENTORY_IDX i = 0; i < INVEN_PACK; i++) {
        o_ptr = &player_ptr->inventory_list[i];
        if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name))) {
            char buf[MAX_NLEN + 20];
            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%s を換金しますか？", "Convert %s into money? "), o_name);
            if (get_check(buf)) {
                msg_format(_("賞金 %ld＄を手に入れた。", "You get %ldgp."), (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
                player_ptr->au += (r_info[today_mon].level * 50 + 100) * o_ptr->number;
                player_ptr->redraw |= (PR_GOLD);
                vary_item(player_ptr, i, -o_ptr->number);
            }

            change = TRUE;
        }
    }

    for (INVENTORY_IDX i = 0; i < INVEN_PACK; i++) {
        o_ptr = &player_ptr->inventory_list[i];

        if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name))) {
            char buf[MAX_NLEN + 20];
            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%s を換金しますか？", "Convert %s into money? "), o_name);
            if (get_check(buf)) {
                msg_format(_("賞金 %ld＄を手に入れた。", "You get %ldgp."), (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
                player_ptr->au += (r_info[today_mon].level * 30 + 60) * o_ptr->number;
                player_ptr->redraw |= (PR_GOLD);
                vary_item(player_ptr, i, -o_ptr->number);
            }

            change = TRUE;
        }
    }

    for (int j = 0; j < MAX_BOUNTY; j++) {
        for (INVENTORY_IDX i = INVEN_PACK - 1; i >= 0; i--) {
            o_ptr = &player_ptr->inventory_list[i];
            if ((o_ptr->tval != TV_CORPSE) || (o_ptr->pval != current_world_ptr->bounty_r_idx[j]))
                continue;

            char buf[MAX_NLEN + 20];
            int num, k;
            INVENTORY_IDX item_new;
            object_type forge;

            object_desc(player_ptr, o_name, o_ptr, 0);
            sprintf(buf, _("%sを渡しますか？", "Hand %s over? "), o_name);
            if (!get_check(buf))
                continue;

            vary_item(player_ptr, i, -o_ptr->number);
            chg_virtue(player_ptr, V_JUSTICE, 5);
            current_world_ptr->bounty_r_idx[j] += 10000;

            for (num = 0, k = 0; k < MAX_BOUNTY; k++) {
                if (current_world_ptr->bounty_r_idx[k] >= 10000)
                    num++;
            }

            msg_format(_("これで合計 %d ポイント獲得しました。", "You earned %d point%s total."), num, (num > 1 ? "s" : ""));

            object_prep(&forge, lookup_kind(prize_list[num - 1].tval, prize_list[num - 1].sval));
            apply_magic(player_ptr, &forge, player_ptr->current_floor_ptr->object_level, AM_NO_FIXED_ART);

            object_aware(player_ptr, &forge);
            object_known(&forge);

            /*
			 * Hand it --- Assume there is an empty slot.
			 * Since a corpse is handed at first,
			 * there is at least one empty slot.
			 */
            item_new = inven_carry(player_ptr, &forge);
            object_desc(player_ptr, o_name, &forge, 0);
            msg_format(_("%s(%c)を貰った。", "You get %s (%c). "), o_name, index_to_label(item_new));

            autopick_alter_item(player_ptr, item_new, FALSE);
            handle_stuff(player_ptr);
            change = TRUE;
        }
    }

    if (change)
        return TRUE;

    msg_print(_("賞金を得られそうなものは持っていなかった。", "You have nothing."));
    msg_print(NULL);
    return FALSE;
}

/*!
 * @brief 本日の賞金首情報を表示する。
 * @param player_ptr プレーヤーへの参照ポインタ
 * @return なし
 */
void today_target(player_type *player_ptr)
{
    char buf[160];
    monster_race *r_ptr = &r_info[today_mon];

    clear_bldg(4, 18);
    c_put_str(TERM_YELLOW, _("本日の賞金首", "Wanted monster that changes from day to day"), 5, 10);
    sprintf(buf, _("ターゲット： %s", "target: %s"), r_name + r_ptr->name);
    c_put_str(TERM_YELLOW, buf, 6, 10);
    sprintf(buf, _("死体 ---- $%d", "corpse   ---- $%d"), (int)r_ptr->level * 50 + 100);
    prt(buf, 8, 10);
    sprintf(buf, _("骨   ---- $%d", "skeleton ---- $%d"), (int)r_ptr->level * 30 + 60);
    prt(buf, 9, 10);
    player_ptr->today_mon = today_mon;
}

/*!
 * @brief ツチノコの賞金首情報を表示する。
 * @return なし
 */
void tsuchinoko(void)
{
    clear_bldg(4, 18);
    c_put_str(TERM_YELLOW, _("一獲千金の大チャンス！！！", "Big chance for quick money!!!"), 5, 10);
    c_put_str(TERM_YELLOW, _("ターゲット：幻の珍獣「ツチノコ」", "target: the rarest animal 'Tsuchinoko'"), 6, 10);
    c_put_str(TERM_WHITE, _("生け捕り ---- $1,000,000", "catch alive ---- $1,000,000"), 8, 10);
    c_put_str(TERM_WHITE, _("死体     ----   $200,000", "corpse      ----   $200,000"), 9, 10);
    c_put_str(TERM_WHITE, _("骨       ----   $100,000", "bones       ----   $100,000"), 10, 10);
}

/*!
 * @brief 通常の賞金首情報を表示する。
 * @return なし
 */
void show_bounty(void)
{
    TERM_LEN y = 0;

    clear_bldg(4, 18);
    prt(_("死体を持ち帰れば報酬を差し上げます。", "Offer a prize when you bring a wanted monster's corpse"), 4, 10);
    c_put_str(TERM_YELLOW, _("現在の賞金首", "Wanted monsters"), 6, 10);

    for (int i = 0; i < MAX_BOUNTY; i++) {
        byte color;
        concptr done_mark;
        monster_race *r_ptr = &r_info[(current_world_ptr->bounty_r_idx[i] > 10000 ? current_world_ptr->bounty_r_idx[i] - 10000 : current_world_ptr->bounty_r_idx[i])];

        if (current_world_ptr->bounty_r_idx[i] > 10000) {
            color = TERM_RED;
            done_mark = _("(済)", "(done)");
        } else {
            color = TERM_WHITE;
            done_mark = "";
        }

        c_prt(color, format("%s %s", r_name + r_ptr->name, done_mark), y + 7, 10);

        y = (y + 1) % 10;
        if (!y && (i < MAX_BOUNTY - 1)) {
            prt(_("何かキーを押してください", "Hit any key."), 0, 0);
            (void)inkey();
            prt("", 0, 0);
            clear_bldg(7, 18);
        }
    }
}

/*!
 * @brief 今日の賞金首を確定する / Determine today's bounty monster
 * @param player_type プレーヤーへの参照ポインタ
 * @return なし
 * @note conv_old is used if loaded 0.0.3 or older save file
 */
void determine_daily_bounty(player_type *player_ptr, bool conv_old)
{
    int max_dl = 3, i;
    if (!conv_old) {
        for (i = 0; i < current_world_ptr->max_d_idx; i++) {
            if (max_dlv[i] < d_info[i].mindepth)
                continue;
            if (max_dl < max_dlv[i])
                max_dl = max_dlv[i];
        }
    } else {
        max_dl = MAX(max_dlv[DUNGEON_ANGBAND], 3);
    }

    get_mon_num_prep(player_ptr, NULL, NULL);

    while (TRUE) {
        today_mon = get_mon_num(player_ptr, max_dl, GMN_ARENA);
        monster_race *r_ptr;
        r_ptr = &r_info[today_mon];

        if (r_ptr->flags1 & RF1_UNIQUE)
            continue;
        if (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2))
            continue;
        if (r_ptr->flags2 & RF2_MULTIPLY)
            continue;
        if ((r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) != (RF9_DROP_CORPSE | RF9_DROP_SKELETON))
            continue;
        if (r_ptr->level < MIN(max_dl / 2, 40))
            continue;
        if (r_ptr->rarity > 10)
            continue;
        break;
    }
}

/*!
 * @brief 賞金首となるユニークを確定する / Determine bounty uniques
 * @param player_ptr プレーヤーへの参照ポインタ
 * @return なし
 */
void determine_bounty_uniques(player_type *player_ptr)
{
    get_mon_num_prep(player_ptr, NULL, NULL);
    for (int i = 0; i < MAX_BOUNTY; i++) {
        while (TRUE) {
            current_world_ptr->bounty_r_idx[i] = get_mon_num(player_ptr, MAX_DEPTH - 1, GMN_ARENA);
            monster_race *r_ptr;
            r_ptr = &r_info[current_world_ptr->bounty_r_idx[i]];

            if (!(r_ptr->flags1 & RF1_UNIQUE))
                continue;

            if (!(r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)))
                continue;

            if (r_ptr->rarity > 100)
                continue;

            if (no_questor_or_bounty_uniques(current_world_ptr->bounty_r_idx[i]))
                continue;

            int j;
            for (j = 0; j < i; j++) {
                if (current_world_ptr->bounty_r_idx[i] == current_world_ptr->bounty_r_idx[j])
                    break;
            }

            if (j == i)
                break;
        }
    }

    for (int i = 0; i < MAX_BOUNTY - 1; i++) {
        for (int j = i; j < MAX_BOUNTY; j++) {
            MONRACE_IDX tmp;
            if (r_info[current_world_ptr->bounty_r_idx[i]].level > r_info[current_world_ptr->bounty_r_idx[j]].level) {
                tmp = current_world_ptr->bounty_r_idx[i];
                current_world_ptr->bounty_r_idx[i] = current_world_ptr->bounty_r_idx[j];
                current_world_ptr->bounty_r_idx[j] = tmp;
            }
        }
    }
}