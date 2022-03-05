﻿#pragma once

#include "system/angband.h"

#include <map>

enum class MonsterRaceId : int16_t;
struct monster_race;
extern std::map<MonsterRaceId, monster_race> r_info;
int calc_monrace_power(monster_race *r_ptr);

class MonsterRace {
public:
    MonsterRace(MonsterRaceId r_idx);

    static MonsterRaceId pick_one_at_random();

    bool is_valid() const;

private:
    MonsterRaceId r_idx;
};
