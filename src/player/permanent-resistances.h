﻿#pragma once

#include "system/angband.h"

struct player_type;
void player_flags(player_type *creature_ptr, TrFlags &flags);
void riding_flags(player_type *creature_ptr, TrFlags &flags, TrFlags &negative_flags);
