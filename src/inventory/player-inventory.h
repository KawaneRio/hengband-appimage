﻿#pragma once

#include "object/tval-types.h"
#include "system/angband.h"
#include "system/object-type-definition.h"

concptr mention_use(player_type *owner_ptr, int i);
concptr describe_use(player_type *owner_ptr, int i);
void display_inventory(player_type *creature_ptr, tval_type tval);
COMMAND_CODE show_inventory(player_type *owner_ptr, int target_item, BIT_FLAGS mode, tval_type tval);
COMMAND_CODE show_equipment(player_type *owner_ptr, int target_item, BIT_FLAGS mode, tval_type tval);
bool can_get_item(player_type *owner_ptr, tval_type tval);
ITEM_NUMBER scan_floor(player_type *owner_ptr, OBJECT_IDX *items, POSITION y, POSITION x, BIT_FLAGS mode, tval_type item_tester_tval);
COMMAND_CODE show_floor(player_type *owner_ptr, int target_item, POSITION y, POSITION x, TERM_LEN *min_width, tval_type item_tester_tval);
void py_pickup_floor(player_type *creature_ptr, bool pickup);
