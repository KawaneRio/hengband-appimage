﻿#include "player/player-status.h"

enum flag_cause {
    FLAG_CAUSE_INVEN_RARM = 0, /*!< アイテムスロット…右手 */
    FLAG_CAUSE_INVEN_LARM = 1, /*!< アイテムスロット…左手 */
    FLAG_CAUSE_INVEN_BOW = 2, /*!< アイテムスロット…射撃 */
    FLAG_CAUSE_INVEN_RIGHT = 3, /*!< アイテムスロット…右手指 */
    FLAG_CAUSE_INVEN_LEFT = 4, /*!< アイテムスロット…左手指 */
    FLAG_CAUSE_INVEN_NECK = 5, /*!< アイテムスロット…首 */
    FLAG_CAUSE_INVEN_LITE = 6, /*!< アイテムスロット…光源 */
    FLAG_CAUSE_INVEN_BODY = 7, /*!< アイテムスロット…体 */
    FLAG_CAUSE_INVEN_OUTER = 8, /*!< アイテムスロット…体の上 */
    FLAG_CAUSE_INVEN_HEAD = 9, /*!< アイテムスロット…頭部 */
    FLAG_CAUSE_INVEN_HANDS = 10, /*!< アイテムスロット…腕部 */
    FLAG_CAUSE_INVEN_FEET = 11, /*!< アイテムスロット…脚部 */
    FLAG_CAUSE_RACE = 12, /*!< 種族上の体得 */
    FLAG_CAUSE_CLASS = 13, /*!< 職業上の体得 */
    FLAG_CAUSE_PERSONALITY = 14, /*!< 性格上の体得 */
    FLAG_CAUSE_MAGIC_TIME_EFFECT = 15, /*!< 魔法による時限効果 */
    FLAG_CAUSE_MUTATION = 16, /*!< 変異による効果 */
    FLAG_CAUSE_BATTLE_FORM = 17, /*!< 構えによる効果 */
    FLAG_CAUSE_MAX = 18
};

bool is_pass_wall(player_type *creature_ptr);
bool is_kill_wall(player_type *creature_ptr);
BIT_FLAGS is_xtra_might(player_type *creature_ptr);
BIT_FLAGS is_esp_evil(player_type *creature_ptr);
BIT_FLAGS is_esp_animal(player_type *creature_ptr);
BIT_FLAGS is_esp_undead(player_type *creature_ptr);
BIT_FLAGS is_esp_demon(player_type *creature_ptr);
BIT_FLAGS is_esp_orc(player_type *creature_ptr);
BIT_FLAGS is_esp_troll(player_type *creature_ptr);
BIT_FLAGS is_esp_giant(player_type *creature_ptr);
BIT_FLAGS is_esp_dragon(player_type *creature_ptr);
BIT_FLAGS is_esp_human(player_type *creature_ptr);
BIT_FLAGS is_esp_good(player_type *creature_ptr);
BIT_FLAGS is_esp_nonliving(player_type *creature_ptr);
BIT_FLAGS is_esp_unique(player_type *creature_ptr);
BIT_FLAGS is_esp_telepathy(player_type *creature_ptr);
BIT_FLAGS has_bless_blade(player_type *creature_ptr);
BIT_FLAGS has_easy2_weapon(player_type *creature_ptr);
BIT_FLAGS has_down_saving(player_type *creature_ptr);
BIT_FLAGS has_no_ac(player_type *creature_ptr);
void has_no_flowed(player_type *creature_ptr);
BIT_FLAGS has_mighty_throw(player_type *creature_ptr);
BIT_FLAGS has_dec_mana(player_type *creature_ptr);
BIT_FLAGS has_reflect(player_type *creature_ptr);
BIT_FLAGS has_see_nocto(player_type *creature_ptr);
BIT_FLAGS has_warning(player_type *creature_ptr);
BIT_FLAGS has_anti_magic(player_type *creature_ptr);
BIT_FLAGS has_anti_tele(player_type *creature_ptr);
BIT_FLAGS has_sh_fire(player_type *creature_ptr);
BIT_FLAGS has_sh_elec(player_type *creature_ptr);
BIT_FLAGS has_sh_cold(player_type *creature_ptr);
BIT_FLAGS has_easy_spell(player_type *creature_ptr);
BIT_FLAGS has_heavy_spell(player_type *creature_ptr);
BIT_FLAGS has_hold_exp(player_type *creature_ptr);
BIT_FLAGS has_see_inv(player_type *creature_ptr);
BIT_FLAGS has_free_act(player_type *creature_ptr);
BIT_FLAGS is_sustain_str(player_type *creature_ptr);
BIT_FLAGS is_sustain_int(player_type *creature_ptr);
BIT_FLAGS is_sustain_wis(player_type *creature_ptr);
BIT_FLAGS is_sustain_dex(player_type *creature_ptr);
BIT_FLAGS is_sustain_con(player_type *creature_ptr);
BIT_FLAGS is_sustain_chr(player_type *creature_ptr);
BIT_FLAGS has_levitation(player_type *creature_ptr);
void has_can_swim(player_type *creature_ptr);
BIT_FLAGS has_slow_digest(player_type *creature_ptr);
BIT_FLAGS has_regenerate(player_type *creature_ptr);
void has_curses(player_type *creature_ptr);
BIT_FLAGS has_impact(player_type *creature_ptr);
void has_extra_blow(player_type *creature_ptr);
BIT_FLAGS is_resist_acid(player_type *creature_ptr);
BIT_FLAGS is_vuln_acid(player_type *creature_ptr);
PERCENTAGE calc_vuln_acid_rate(player_type *creature_ptr);
BIT_FLAGS is_resist_elec(player_type *creature_ptr);
BIT_FLAGS is_vuln_elec(player_type *creature_ptr);
PERCENTAGE calc_vuln_elec_rate(player_type *creature_ptr);
BIT_FLAGS is_resist_fire(player_type *creature_ptr);
BIT_FLAGS is_vuln_fire(player_type *creature_ptr);
BIT_FLAGS is_resist_cold(player_type *creature_ptr);
BIT_FLAGS is_vuln_cold(player_type *creature_ptr);
BIT_FLAGS is_resist_pois(player_type *creature_ptr);
BIT_FLAGS is_resist_conf(player_type *creature_ptr);
BIT_FLAGS is_resist_sound(player_type *creature_ptr);
BIT_FLAGS is_resist_lite(player_type *creature_ptr);
BIT_FLAGS is_vuln_lite(player_type *creature_ptr);
BIT_FLAGS is_resist_dark(player_type *creature_ptr);
BIT_FLAGS is_resist_chaos(player_type *creature_ptr);
BIT_FLAGS is_resist_disen(player_type *creature_ptr);
BIT_FLAGS is_resist_shard(player_type *creature_ptr);
BIT_FLAGS is_resist_nexus(player_type *creature_ptr);
BIT_FLAGS is_resist_blind(player_type *creature_ptr);
BIT_FLAGS is_resist_neth(player_type *creature_ptr);
BIT_FLAGS is_resist_time(player_type *creature_ptr);
BIT_FLAGS is_resist_water(player_type *creature_ptr);
BIT_FLAGS is_resist_fear(player_type *creature_ptr);
BIT_FLAGS is_immune_acid(player_type *creature_ptr);
BIT_FLAGS is_immune_elec(player_type *creature_ptr);
BIT_FLAGS is_immune_fire(player_type *creature_ptr);
BIT_FLAGS is_immune_cold(player_type *creature_ptr);
bool has_right_hand_weapon(player_type *creature_ptr);
bool has_left_hand_weapon(player_type *creature_ptr);
bool has_two_handed_weapons(player_type *creature_ptr);
BIT_FLAGS has_lite(player_type *creature_ptr);
bool is_disable_two_handed_bonus(player_type *creature_ptr, int i);
bool is_not_ninja_weapon(player_type *creature_ptr, int i);
bool is_not_monk_weapon(player_type *creature_ptr, int i);
bool is_icky_wield_weapon(player_type *creature_ptr, int i);
bool is_riding_wield_weapon(player_type *creature_ptr, int i);
bool has_good_luck(player_type *creature_ptr);
