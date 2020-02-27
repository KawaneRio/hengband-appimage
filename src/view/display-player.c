﻿/*!
 * @brief プレーヤーのステータス表示メインルーチン群
 * @date 2020/02/25
 * @author Hourier
 * @details
 * ここにこれ以上関数を引っ越してくるのは禁止。何ならここから更に分割していく
 */

#include "display-player.h"
#include "player-personality.h"
#include "term.h"
#include "status-first-page.h"
#include "player-sex.h"
#include "patron.h"
#include "world.h"
#include "quest.h"
#include "core.h" // 暫定。後で消す
#include "player/permanent-resistances.h" // 暫定。後で消す
#include "mutation.h"
#include "player-skill.h"
#include "player-effects.h"
#include "realm-song.h"
#include "object-hook.h"
#include "shoot.h"
#include "dungeon-file.h"
#include "objectkind.h"
#include "view/display-util.h"
#include "view/display-characteristic.h"

/*!
 * @brief プレイヤーの特性フラグ一覧表示2a /
 * @param creature_ptr プレーヤーへの参照ポインタ
 * Special display, part 2a
 * @return なし
 */
static void display_player_misc_info(player_type *creature_ptr)
{
#ifdef JP
	put_str("名前  :", 1, 26);
	put_str("性別  :", 3, 1);
	put_str("種族  :", 4, 1);
	put_str("職業  :", 5, 1);
#else
	put_str("Name  :", 1, 26);
	put_str("Sex   :", 3, 1);
	put_str("Race  :", 4, 1);
	put_str("Class :", 5, 1);
#endif

	char	buf[80];
	char	tmp[80];
	strcpy(tmp, ap_ptr->title);
#ifdef JP
	if (ap_ptr->no == 1)
		strcat(tmp, "の");
#else
	strcat(tmp, " ");
#endif
	strcat(tmp, creature_ptr->name);

	c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 9);
	c_put_str(TERM_L_BLUE, (creature_ptr->mimic_form ? mimic_info[creature_ptr->mimic_form].title : rp_ptr->title), 4, 9);
	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 9);

#ifdef JP
	put_str("レベル:", 6, 1);
	put_str("ＨＰ  :", 7, 1);
	put_str("ＭＰ  :", 8, 1);
#else
	put_str("Level :", 6, 1);
	put_str("Hits  :", 7, 1);
	put_str("Mana  :", 8, 1);
#endif

	(void)sprintf(buf, "%d", (int)creature_ptr->lev);
	c_put_str(TERM_L_BLUE, buf, 6, 9);
	(void)sprintf(buf, "%d/%d", (int)creature_ptr->chp, (int)creature_ptr->mhp);
	c_put_str(TERM_L_BLUE, buf, 7, 9);
	(void)sprintf(buf, "%d/%d", (int)creature_ptr->csp, (int)creature_ptr->msp);
	c_put_str(TERM_L_BLUE, buf, 8, 9);
}


/*!
 * @brief プレーヤーのパラメータ基礎値 (腕力等)を18以下になるようにして返す
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param stat_num パラメータ番号
 * @return 基礎値
 * @details 最大が18になるのはD&D由来
 */
static int calc_basic_stat(player_type *creature_ptr, int stat_num)
{
	int e_adj = 0;
	if ((creature_ptr->stat_max[stat_num] > 18) && (creature_ptr->stat_top[stat_num] > 18))
		e_adj = (creature_ptr->stat_top[stat_num] - creature_ptr->stat_max[stat_num]) / 10;

	if ((creature_ptr->stat_max[stat_num] <= 18) && (creature_ptr->stat_top[stat_num] <= 18))
		e_adj = creature_ptr->stat_top[stat_num] - creature_ptr->stat_max[stat_num];

	if ((creature_ptr->stat_max[stat_num] <= 18) && (creature_ptr->stat_top[stat_num] > 18))
		e_adj = (creature_ptr->stat_top[stat_num] - 18) / 10 - creature_ptr->stat_max[stat_num] + 18;

	if ((creature_ptr->stat_max[stat_num] > 18) && (creature_ptr->stat_top[stat_num] <= 18))
		e_adj = creature_ptr->stat_top[stat_num] - (creature_ptr->stat_max[stat_num] - 19) / 10 - 19;

	return e_adj;
}


/*!
 * @brief 特殊な種族の時、腕力等の基礎パラメータを変動させる
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param stat_num パラメータ番号
 * @return 補正後の基礎パラメータ
 */
static int compensate_special_race(player_type *creature_ptr, int stat_num)
{
	if (!PRACE_IS_(creature_ptr, RACE_ENT)) return 0;

	int r_adj = 0;
	switch (stat_num)
	{
	case A_STR:
	case A_CON:
		if (creature_ptr->lev > 25) r_adj++;
		if (creature_ptr->lev > 40) r_adj++;
		if (creature_ptr->lev > 45) r_adj++;
		break;
	case A_DEX:
		if (creature_ptr->lev > 25) r_adj--;
		if (creature_ptr->lev > 40) r_adj--;
		if (creature_ptr->lev > 45) r_adj--;
		break;
	}

	return r_adj;
}


/*!
 * @brief 能力値名を(もし一時的減少なら'x'を付けて)表示する
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param stat_num パラメータ番号
 * @param row 行数
 * @param stat_col 列数
 * @return なし
 */
static void display_basic_stat_name(player_type *creature_ptr, int stat_num, int row, int stat_col)
{
	if (creature_ptr->stat_cur[stat_num] < creature_ptr->stat_max[stat_num])
		c_put_str(TERM_WHITE, stat_names_reduced[stat_num], row + stat_num + 1, stat_col + 1);
	else
		c_put_str(TERM_WHITE, stat_names[stat_num], row + stat_num + 1, stat_col + 1);
}


/*!
 * @brief 能力値を、基本・種族補正・職業補正・性格補正・装備補正・合計・現在 (一時的減少のみ) の順で表示する
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param stat_num パラメータ番号
 * @param r_adj 補正後の基礎パラメータ
 * @param e_adj 種族補正値
 * @param row 行数
 * @param stat_col 列数
 * @param buf 能力値の数値
 * @return なし
 */
static void display_basic_stat_value(player_type *creature_ptr, int stat_num, int r_adj, int e_adj, int row, int stat_col, char *buf)
{
	(void)sprintf(buf, "%3d", r_adj);
	c_put_str(TERM_L_BLUE, buf, row + stat_num + 1, stat_col + 13);

	(void)sprintf(buf, "%3d", (int)cp_ptr->c_adj[stat_num]);
	c_put_str(TERM_L_BLUE, buf, row + stat_num + 1, stat_col + 16);

	(void)sprintf(buf, "%3d", (int)ap_ptr->a_adj[stat_num]);
	c_put_str(TERM_L_BLUE, buf, row + stat_num + 1, stat_col + 19);

	(void)sprintf(buf, "%3d", (int)e_adj);
	c_put_str(TERM_L_BLUE, buf, row + stat_num + 1, stat_col + 22);

	cnv_stat(creature_ptr->stat_top[stat_num], buf);
	c_put_str(TERM_L_GREEN, buf, row + stat_num + 1, stat_col + 26);

	if (creature_ptr->stat_use[stat_num] < creature_ptr->stat_top[stat_num])
	{
		cnv_stat(creature_ptr->stat_use[stat_num], buf);
		c_put_str(TERM_YELLOW, buf, row + stat_num + 1, stat_col + 33);
	}
}


/*!
 * @brief 能力値を補正しつつ表示する
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param row 行数
 * @param stat_col 列数
 * @return なし
 */
static void process_stats(player_type *creature_ptr, int row, int stat_col)
{
	char buf[80];
	for (int i = 0; i < A_MAX; i++)
	{
		int r_adj = creature_ptr->mimic_form
			? mimic_info[creature_ptr->mimic_form].r_adj[i]
			: rp_ptr->r_adj[i];
		int e_adj = calc_basic_stat(creature_ptr, i);
		r_adj += compensate_special_race(creature_ptr, i);
		e_adj -= r_adj;
		e_adj -= cp_ptr->c_adj[i];
		e_adj -= ap_ptr->a_adj[i];

		display_basic_stat_name(creature_ptr, i, row, stat_col);
		cnv_stat(creature_ptr->stat_max[i], buf);
		if (creature_ptr->stat_max[i] == creature_ptr->stat_max_max[i])
			c_put_str(TERM_WHITE, "!", row + i + 1, _(stat_col + 6, stat_col + 4));

		c_put_str(TERM_BLUE, buf, row + i + 1, stat_col + 13 - strlen(buf));

		display_basic_stat_value(creature_ptr, i, r_adj, e_adj, row, stat_col, buf);
	}
}


/*!
 * @brief プレイヤーの特性フラグ一覧表示2b /
 * Special display, part 2b
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @return なし
 * @details
 * <pre>
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 * </pre>
 */
static void display_player_stat_info(player_type *creature_ptr)
{
	int stat_col = 22;
	int row = 3;
	c_put_str(TERM_WHITE, _("能力", "Stat"), row, stat_col + 1);
	c_put_str(TERM_BLUE, _("  基本", "  Base"), row, stat_col + 7);
	c_put_str(TERM_L_BLUE, _(" 種 職 性 装 ", "RacClaPerMod"), row, stat_col + 13);
	c_put_str(TERM_L_GREEN, _("合計", "Actual"), row, stat_col + 28);
	c_put_str(TERM_YELLOW, _("現在", "Current"), row, stat_col + 35);
	process_stats(creature_ptr, row, stat_col);

	int col = stat_col + 41;
	c_put_str(TERM_WHITE, "abcdefghijkl@", row, col);
	c_put_str(TERM_L_GREEN, _("能力修正", "Modification"), row - 1, col);

	BIT_FLAGS flags[TR_FLAG_SIZE];
	for (int i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr;
		o_ptr = &creature_ptr->inventory_list[i];
		object_flags_known(o_ptr, flags);
		for (int stat = 0; stat < A_MAX; stat++)
		{
			byte a = TERM_SLATE;
			char c = '.';
			if (have_flag(flags, stat))
			{
				c = '*';

				if (o_ptr->pval > 0)
				{
					a = TERM_L_GREEN;
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				if (have_flag(flags, stat + TR_SUST_STR))
				{
					a = TERM_GREEN;
				}

				if (o_ptr->pval < 0)
				{
					a = TERM_RED;
					if (o_ptr->pval > -10) c = '0' - o_ptr->pval;
				}
			}
			else if (have_flag(flags, stat + TR_SUST_STR))
			{
				a = TERM_GREEN;
				c = 's';
			}

			Term_putch(col, row + stat + 1, a, c);
		}

		col++;
	}

	player_flags(creature_ptr, flags);
	for (int stat = 0; stat < A_MAX; stat++)
	{
		byte a = TERM_SLATE;
		char c = '.';

		if (creature_ptr->muta3 || creature_ptr->tsuyoshi)
		{
			int dummy = 0;

			if (stat == A_STR)
			{
				if (creature_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (creature_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (creature_ptr->tsuyoshi) dummy += 4;
			}
			else if (stat == A_WIS || stat == A_INT)
			{
				if (creature_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (creature_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
			}
			else if (stat == A_DEX)
			{
				if (creature_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (creature_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (creature_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
			}
			else if (stat == A_CON)
			{
				if (creature_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (creature_ptr->muta3 & MUT3_XTRA_FAT) dummy += 2;
				if (creature_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (creature_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 2;
				if (creature_ptr->tsuyoshi) dummy += 4;
			}
			else if (stat == A_CHR)
			{
				if (creature_ptr->muta3 & MUT3_SILLY_VOI) dummy -= 4;
				if (creature_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 1;
				if (creature_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 1;
				if (creature_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				if (creature_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				if (creature_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
			}

			if (dummy != 0)
			{
				c = '*';
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				if (dummy < 0)
				{
					a = TERM_RED;
					if (dummy > -10) c = '0' - dummy;
				}
			}
		}

		if (have_flag(flags, stat + TR_SUST_STR))
		{
			a = TERM_GREEN;
			c = 's';
		}

		Term_putch(col, row + stat + 1, a, c);
	}
}


/*!
 * @brief プレイヤーの打撃能力修正を表示する
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param hand 武器の装備部位ID
 * @param hand_entry 項目ID
 * @return なし
 */
static void display_player_melee_bonus(player_type *creature_ptr, int hand, int hand_entry)
{
	HIT_PROB show_tohit = creature_ptr->dis_to_h[hand];
	HIT_POINT show_todam = creature_ptr->dis_to_d[hand];
	object_type *o_ptr = &creature_ptr->inventory_list[INVEN_RARM + hand];

	if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

	show_tohit += creature_ptr->skill_thn / BTH_PLUS_ADJ;

	char buf[160];
	sprintf(buf, "(%+d,%+d)", (int)show_tohit, (int)show_todam);

	if (!has_melee_weapon(creature_ptr, INVEN_RARM) && !has_melee_weapon(creature_ptr, INVEN_LARM))
		display_player_one_line(ENTRY_BARE_HAND, buf, TERM_L_BLUE);
	else if (creature_ptr->ryoute)
		display_player_one_line(ENTRY_TWO_HANDS, buf, TERM_L_BLUE);
	else
		display_player_one_line(hand_entry, buf, TERM_L_BLUE);
}


/*!
 * @brief プレイヤーステータス表示の中央部分を表示するサブルーチン
 * @param creature_ptr プレーヤーへの参照ポインタ
 * Prints the following information on the screen.
 * @return なし
 */
static void display_player_middle(player_type *creature_ptr)
{
	HIT_PROB show_tohit = creature_ptr->dis_to_h_b;
	HIT_POINT show_todam = 0;
	if (creature_ptr->migite)
	{
		display_player_melee_bonus(creature_ptr, 0, left_hander ? ENTRY_LEFT_HAND1 : ENTRY_RIGHT_HAND1);
	}

	if (creature_ptr->hidarite)
	{
		display_player_melee_bonus(creature_ptr, 1, left_hander ? ENTRY_RIGHT_HAND2 : ENTRY_LEFT_HAND2);
	}
	else if ((creature_ptr->pclass == CLASS_MONK) && (empty_hands(creature_ptr, TRUE) & EMPTY_HAND_RARM))
	{
		int i;
		if (creature_ptr->special_defense & KAMAE_MASK)
		{
			for (i = 0; i < MAX_KAMAE; i++)
			{
				if ((creature_ptr->special_defense >> i) & KAMAE_GENBU) break;
			}
			if (i < MAX_KAMAE)
			{
				display_player_one_line(ENTRY_POSTURE, format(_("%sの構え", "%s form"), kamae_shurui[i].desc), TERM_YELLOW);
			}
		}
		else
		{
			display_player_one_line(ENTRY_POSTURE, _("構えなし", "none"), TERM_YELLOW);
		}
	}

	object_type *o_ptr = &creature_ptr->inventory_list[INVEN_BOW];
	if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

	if ((o_ptr->sval == SV_LIGHT_XBOW) || (o_ptr->sval == SV_HEAVY_XBOW))
		show_tohit += creature_ptr->weapon_exp[0][o_ptr->sval] / 400;
	else
		show_tohit += (creature_ptr->weapon_exp[0][o_ptr->sval] - (WEAPON_EXP_MASTER / 2)) / 200;

	show_tohit += creature_ptr->skill_thb / BTH_PLUS_ADJ;

	display_player_one_line(ENTRY_SHOOT_HIT_DAM, format("(%+d,%+d)", show_tohit, show_todam), TERM_L_BLUE);
	int tmul = 0;
	if (creature_ptr->inventory_list[INVEN_BOW].k_idx)
	{
		tmul = bow_tmul(creature_ptr->inventory_list[INVEN_BOW].sval);
		if (creature_ptr->xtra_might) tmul++;

		tmul = tmul * (100 + (int)(adj_str_td[creature_ptr->stat_ind[A_STR]]) - 128);
	}

	display_player_one_line(ENTRY_SHOOT_POWER, format("x%d.%02d", tmul / 100, tmul % 100), TERM_L_BLUE);
	display_player_one_line(ENTRY_BASE_AC, format("[%d,%+d]", creature_ptr->dis_ac, creature_ptr->dis_to_a), TERM_L_BLUE);

	int i = creature_ptr->pspeed - 110;
	if (creature_ptr->action == ACTION_SEARCH) i += 10;

	TERM_COLOR attr;
	if (i > 0)
	{
		if (!creature_ptr->riding)
			attr = TERM_L_GREEN;
		else
			attr = TERM_GREEN;
	}
	else if (i == 0)
	{
		if (!creature_ptr->riding)
			attr = TERM_L_BLUE;
		else
			attr = TERM_GREEN;
	}
	else
	{
		if (!creature_ptr->riding)
			attr = TERM_L_UMBER;
		else
			attr = TERM_RED;
	}

	int tmp_speed = 0;
	if (!creature_ptr->riding)
	{
		if (IS_FAST(creature_ptr)) tmp_speed += 10;
		if (creature_ptr->slow) tmp_speed -= 10;
		if (creature_ptr->lightspeed) tmp_speed = 99;
	}
	else
	{
		if (MON_FAST(&creature_ptr->current_floor_ptr->m_list[creature_ptr->riding])) tmp_speed += 10;
		if (MON_SLOW(&creature_ptr->current_floor_ptr->m_list[creature_ptr->riding])) tmp_speed -= 10;
	}

	char buf[160];
	if (tmp_speed)
	{
		if (!creature_ptr->riding)
			sprintf(buf, "(%+d%+d)", i - tmp_speed, tmp_speed);
		else
			sprintf(buf, _("乗馬中 (%+d%+d)", "Riding (%+d%+d)"), i - tmp_speed, tmp_speed);

		if (tmp_speed > 0)
			attr = TERM_YELLOW;
		else
			attr = TERM_VIOLET;
	}
	else
	{
		if (!creature_ptr->riding)
			sprintf(buf, "(%+d)", i);
		else
			sprintf(buf, _("乗馬中 (%+d)", "Riding (%+d)"), i);
	}

	display_player_one_line(ENTRY_SPEED, buf, attr);
	display_player_one_line(ENTRY_LEVEL, format("%d", creature_ptr->lev), TERM_L_GREEN);

	int e = (creature_ptr->prace == RACE_ANDROID) ? ENTRY_EXP_ANDR : ENTRY_CUR_EXP;
	if (creature_ptr->exp >= creature_ptr->max_exp)
		display_player_one_line(e, format("%ld", creature_ptr->exp), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", creature_ptr->exp), TERM_YELLOW);

	if (creature_ptr->prace != RACE_ANDROID)
		display_player_one_line(ENTRY_MAX_EXP, format("%ld", creature_ptr->max_exp), TERM_L_GREEN);

	e = (creature_ptr->prace == RACE_ANDROID) ? ENTRY_EXP_TO_ADV_ANDR : ENTRY_EXP_TO_ADV;

	if (creature_ptr->lev >= PY_MAX_LEVEL)
		display_player_one_line(e, "*****", TERM_L_GREEN);
	else if (creature_ptr->prace == RACE_ANDROID)
		display_player_one_line(e, format("%ld", (s32b)(player_exp_a[creature_ptr->lev - 1] * creature_ptr->expfact / 100L)), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", (s32b)(player_exp[creature_ptr->lev - 1] * creature_ptr->expfact / 100L)), TERM_L_GREEN);

	display_player_one_line(ENTRY_GOLD, format("%ld", creature_ptr->au), TERM_L_GREEN);

	int day, hour, min;
	extract_day_hour_min(creature_ptr, &day, &hour, &min);

	if (day < MAX_DAYS)
		sprintf(buf, _("%d日目 %2d:%02d", "Day %d %2d:%02d"), day, hour, min);
	else
		sprintf(buf, _("*****日目 %2d:%02d", "Day ***** %2d:%02d"), hour, min);

	display_player_one_line(ENTRY_DAY, buf, TERM_L_GREEN);

	if (creature_ptr->chp >= creature_ptr->mhp)
		display_player_one_line(ENTRY_HP, format("%4d/%4d", creature_ptr->chp, creature_ptr->mhp), TERM_L_GREEN);
	else if (creature_ptr->chp > (creature_ptr->mhp * hitpoint_warn) / 10)
		display_player_one_line(ENTRY_HP, format("%4d/%4d", creature_ptr->chp, creature_ptr->mhp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_HP, format("%4d/%4d", creature_ptr->chp, creature_ptr->mhp), TERM_RED);

	if (creature_ptr->csp >= creature_ptr->msp)
		display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp, creature_ptr->msp), TERM_L_GREEN);
	else if (creature_ptr->csp > (creature_ptr->msp * mana_warn) / 10)
		display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp, creature_ptr->msp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp, creature_ptr->msp), TERM_RED);

	u32b play_hour = current_world_ptr->play_time / (60 * 60);
	u32b play_min = (current_world_ptr->play_time / 60) % 60;
	u32b play_sec = current_world_ptr->play_time % 60;
	display_player_one_line(ENTRY_PLAY_TIME, format("%.2lu:%.2lu:%.2lu", play_hour, play_min, play_sec), TERM_L_GREEN);
}


/*!
 * @brief プレイヤーのステータス表示メイン処理
 * Display the character on the screen (various modes)
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param mode 表示モードID
 * @return なし
 * @details
 * <pre>
 * The top one and bottom two lines are left blank.
 * Mode 0 = standard display with skills
 * Mode 1 = standard display with history
 * Mode 2 = summary of various things
 * Mode 3 = summary of various things (part 2)
 * Mode 4 = mutations
 * </pre>
 */
void display_player(player_type *creature_ptr, int mode, map_name_pf map_name)
{
	if ((creature_ptr->muta1 || creature_ptr->muta2 || creature_ptr->muta3) && display_mutations)
		mode = (mode % 5);
	else
		mode = (mode % 4);

	clear_from(0);

	if (mode == 2)
	{
		display_player_misc_info(creature_ptr);
		display_player_stat_info(creature_ptr);
		display_player_flag_info_1(creature_ptr, display_player_equippy);
		return;
	}

	if (mode == 3)
	{
		display_player_flag_info_2(creature_ptr, display_player_equippy);
		return;
	}

	if (mode == 4)
	{
		do_cmd_knowledge_mutations(creature_ptr);
		return;
	}

	char tmp[64];
	if ((mode != 0) && (mode != 1)) return;

	/* Name, Sex, Race, Class */
#ifdef JP
	sprintf(tmp, "%s%s%s", ap_ptr->title, ap_ptr->no == 1 ? "の" : "", creature_ptr->name);
#else
	sprintf(tmp, "%s %s", ap_ptr->title, creature_ptr->name);
#endif

	display_player_one_line(ENTRY_NAME, tmp, TERM_L_BLUE);
	display_player_one_line(ENTRY_SEX, sp_ptr->title, TERM_L_BLUE);
	display_player_one_line(ENTRY_RACE, (creature_ptr->mimic_form ? mimic_info[creature_ptr->mimic_form].title : rp_ptr->title), TERM_L_BLUE);
	display_player_one_line(ENTRY_CLASS, cp_ptr->title, TERM_L_BLUE);

	if (creature_ptr->realm1)
	{
		if (creature_ptr->realm2)
			sprintf(tmp, "%s, %s", realm_names[creature_ptr->realm1], realm_names[creature_ptr->realm2]);
		else
			strcpy(tmp, realm_names[creature_ptr->realm1]);
		display_player_one_line(ENTRY_REALM, tmp, TERM_L_BLUE);
	}

	if ((creature_ptr->pclass == CLASS_CHAOS_WARRIOR) || (creature_ptr->muta2 & MUT2_CHAOS_GIFT))
		display_player_one_line(ENTRY_PATRON, chaos_patrons[creature_ptr->chaos_patron], TERM_L_BLUE);

	/* Age, Height, Weight, Social */
	/* 身長はセンチメートルに、体重はキログラムに変更してあります */
#ifdef JP
	display_player_one_line(ENTRY_AGE, format("%d才", (int)creature_ptr->age), TERM_L_BLUE);
	display_player_one_line(ENTRY_HEIGHT, format("%dcm", (int)((creature_ptr->ht * 254) / 100)), TERM_L_BLUE);
	display_player_one_line(ENTRY_WEIGHT, format("%dkg", (int)((creature_ptr->wt * 4536) / 10000)), TERM_L_BLUE);
	display_player_one_line(ENTRY_SOCIAL, format("%d  ", (int)creature_ptr->sc), TERM_L_BLUE);
#else
	display_player_one_line(ENTRY_AGE, format("%d", (int)creature_ptr->age), TERM_L_BLUE);
	display_player_one_line(ENTRY_HEIGHT, format("%d", (int)creature_ptr->ht), TERM_L_BLUE);
	display_player_one_line(ENTRY_WEIGHT, format("%d", (int)creature_ptr->wt), TERM_L_BLUE);
	display_player_one_line(ENTRY_SOCIAL, format("%d", (int)creature_ptr->sc), TERM_L_BLUE);
#endif
	display_player_one_line(ENTRY_ALIGN, format("%s", your_alignment(creature_ptr)), TERM_L_BLUE);

	char buf[80];
	for (int i = 0; i < A_MAX; i++)
	{
		if (creature_ptr->stat_cur[i] < creature_ptr->stat_max[i])
		{
			put_str(stat_names_reduced[i], 3 + i, 53);
			int value = creature_ptr->stat_use[i];
			cnv_stat(value, buf);
			c_put_str(TERM_YELLOW, buf, 3 + i, 60);
			value = creature_ptr->stat_top[i];
			cnv_stat(value, buf);
			c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
		}
		else
		{
			put_str(stat_names[i], 3 + i, 53);
			cnv_stat(creature_ptr->stat_use[i], buf);
			c_put_str(TERM_L_GREEN, buf, 3 + i, 60);
		}

		if (creature_ptr->stat_max[i] == creature_ptr->stat_max_max[i])
		{
			c_put_str(TERM_WHITE, "!", 3 + i, _(58, 58 - 2));
		}
	}

	floor_type *floor_ptr = creature_ptr->current_floor_ptr;
	if (mode == 0)
	{
		display_player_middle(creature_ptr);
		display_player_various(creature_ptr);
		return;
	}

	char statmsg[1000];
	put_str(_("(キャラクターの生い立ち)", "(Character Background)"), 11, 25);

	for (int i = 0; i < 4; i++)
	{
		put_str(creature_ptr->history[i], i + 12, 10);
	}

	*statmsg = '\0';

	if (creature_ptr->is_dead)
	{
		if (current_world_ptr->total_winner)
		{
#ifdef JP
			sprintf(statmsg, "…あなたは勝利の後%sした。", streq(creature_ptr->died_from, "Seppuku") ? "切腹" : "引退");
#else
			sprintf(statmsg, "...You %s after winning.", streq(creature_ptr->died_from, "Seppuku") ? "committed seppuku" : "retired from the adventure");
#endif
		}
		else if (!floor_ptr->dun_level)
		{
#ifdef JP
			sprintf(statmsg, "…あなたは%sで%sに殺された。", (*map_name)(creature_ptr), creature_ptr->died_from);
#else
			sprintf(statmsg, "...You were killed by %s in %s.", creature_ptr->died_from, map_name(creature_ptr));
#endif
		}
		else if (floor_ptr->inside_quest && is_fixed_quest_idx(floor_ptr->inside_quest))
		{
			/* Get the quest text */
			/* Bewere that INIT_ASSIGN resets the cur_num. */
			init_flags = INIT_NAME_ONLY;

			process_dungeon_file(creature_ptr, "q_info.txt", 0, 0, 0, 0);

#ifdef JP
			sprintf(statmsg, "…あなたは、クエスト「%s」で%sに殺された。", quest[floor_ptr->inside_quest].name, creature_ptr->died_from);
#else
			sprintf(statmsg, "...You were killed by %s in the quest '%s'.", creature_ptr->died_from, quest[floor_ptr->inside_quest].name);
#endif
		}
		else
		{
#ifdef JP
			sprintf(statmsg, "…あなたは、%sの%d階で%sに殺された。", (*map_name)(creature_ptr), (int)floor_ptr->dun_level, creature_ptr->died_from);
#else
			sprintf(statmsg, "...You were killed by %s on level %d of %s.", creature_ptr->died_from, floor_ptr->dun_level, map_name(creature_ptr));
#endif
		}
	}
	else if (current_world_ptr->character_dungeon)
	{
		if (!floor_ptr->dun_level)
		{
			sprintf(statmsg, _("…あなたは現在、 %s にいる。", "...Now, you are in %s."), map_name(creature_ptr));
		}
		else if (floor_ptr->inside_quest && is_fixed_quest_idx(floor_ptr->inside_quest))
		{
			/* Clear the text */
			/* Must be done before doing INIT_SHOW_TEXT */
			for (int i = 0; i < 10; i++)
			{
				quest_text[i][0] = '\0';
			}

			quest_text_line = 0;
			init_flags = INIT_NAME_ONLY;
			process_dungeon_file(creature_ptr, "q_info.txt", 0, 0, 0, 0);
			sprintf(statmsg, _("…あなたは現在、 クエスト「%s」を遂行中だ。", "...Now, you are in the quest '%s'."), quest[floor_ptr->inside_quest].name);
		}
		else
		{
#ifdef JP
			sprintf(statmsg, "…あなたは現在、 %s の %d 階で探索している。", map_name(creature_ptr), (int)floor_ptr->dun_level);
#else
			sprintf(statmsg, "...Now, you are exploring level %d of %s.", floor_ptr->dun_level, map_name(creature_ptr));
#endif
		}
	}

	if (!*statmsg) return;

	char temp[64 * 2];
	roff_to_buf(statmsg, 60, temp, sizeof(temp));
	char  *t;
	t = temp;
	for (int i = 0; i < 2; i++)
	{
		if (t[0] == 0) return;

		put_str(t, i + 5 + 12, 10);
		t += strlen(t) + 1;
	}
}


/*!
 * todo y = 6、x = 0、mode = 0で固定。何とかする
 * @brief プレイヤーの装備一覧をシンボルで並べる
 * Equippy chars
 * @param creature_ptr プレーヤーへの参照ポインタ
 * @param y 表示するコンソールの行
 * @param x 表示するコンソールの列
 * @param mode オプション
 * @return なし
 */
void display_player_equippy(player_type *creature_ptr, TERM_LEN y, TERM_LEN x, BIT_FLAGS16 mode)
{
	int max_i = (mode & DP_WP) ? INVEN_LARM + 1 : INVEN_TOTAL;
	for (int i = INVEN_RARM; i < max_i; i++)
	{
		object_type *o_ptr;
		o_ptr = &creature_ptr->inventory_list[i];

		TERM_COLOR a = object_attr(o_ptr);
		char c = object_char(o_ptr);

		if (!equippy_chars || !o_ptr->k_idx)
		{
			c = ' ';
			a = TERM_DARK;
		}

		Term_putch(x + i - INVEN_RARM, y, a, c);
	}
}
