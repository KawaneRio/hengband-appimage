﻿#pragma once

#include "system/angband.h"
#include "effect/attribute-types.h"

class PlayerType;
bool affect_monster(PlayerType *player_ptr, MONSTER_IDX who, POSITION r, POSITION y, POSITION x, int dam, AttributeType typ, BIT_FLAGS flag, bool see_s_msg);
