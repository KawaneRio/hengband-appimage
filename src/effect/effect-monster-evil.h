﻿#pragma once

#include "system/angband.h"
#include "effect/effect-monster-util.h"

gf_switch_result effect_monster_away_undead(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_away_evil(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_away_all(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_turn_undead(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_turn_evil(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_turn_all(effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_undead(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_evil(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_good(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_living(effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_demon(player_type *caster_ptr, effect_monster_type *em_ptr);
gf_switch_result effect_monster_disp_all(effect_monster_type *em_ptr);
