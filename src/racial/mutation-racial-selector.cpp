﻿#include "racial/mutation-racial-selector.h"
#include "mutation/mutation-flag-types.h"
#include "racial/racial-util.h"

void select_mutation_racial(player_type *creature_ptr, rc_type *rc_ptr)
{
    if (creature_ptr->muta.has(MUTA::SPIT_ACID)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("酸の唾", "Spit Acid");
        rc_ptr->power_desc[rc_ptr->num].min_level = 9;
        rc_ptr->power_desc[rc_ptr->num].cost = 9;
        rc_ptr->power_desc[rc_ptr->num].stat = A_DEX;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::SPIT_ACID);
    }

    if (creature_ptr->muta.has(MUTA::BR_FIRE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("炎のブレス", "Fire Breath");
        rc_ptr->power_desc[rc_ptr->num].min_level = 20;
        rc_ptr->power_desc[rc_ptr->num].cost = rc_ptr->lvl;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 18;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::BR_FIRE);
    }

    if (creature_ptr->muta.has(MUTA::HYPN_GAZE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("催眠睨み", "Hypnotic Gaze");
        rc_ptr->power_desc[rc_ptr->num].min_level = 12;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CHR;
        rc_ptr->power_desc[rc_ptr->num].fail = 18;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::HYPN_GAZE);
    }

    if (creature_ptr->muta.has(MUTA::TELEKINES)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("念動力", "Telekinesis");
        rc_ptr->power_desc[rc_ptr->num].min_level = 9;
        rc_ptr->power_desc[rc_ptr->num].cost = 9;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::TELEKINES);
    }

    if (creature_ptr->muta.has(MUTA::VTELEPORT)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("テレポート", "Teleport");
        rc_ptr->power_desc[rc_ptr->num].min_level = 7;
        rc_ptr->power_desc[rc_ptr->num].cost = 7;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::VTELEPORT);
    }

    if (creature_ptr->muta.has(MUTA::MIND_BLST)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("精神攻撃", "Mind Blast");
        rc_ptr->power_desc[rc_ptr->num].min_level = 5;
        rc_ptr->power_desc[rc_ptr->num].cost = 3;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::MIND_BLST);
    }

    if (creature_ptr->muta.has(MUTA::RADIATION)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("放射能", "Emit Radiation");
        rc_ptr->power_desc[rc_ptr->num].min_level = 15;
        rc_ptr->power_desc[rc_ptr->num].cost = 15;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::RADIATION);
    }

    if (creature_ptr->muta.has(MUTA::VAMPIRISM)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("吸血", "Vampiric Drain");
        rc_ptr->power_desc[rc_ptr->num].min_level = 2;
        rc_ptr->power_desc[rc_ptr->num].cost = 1 + (rc_ptr->lvl / 3);
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 9;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::VAMPIRISM);
    }

    if (creature_ptr->muta.has(MUTA::SMELL_MET)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("金属嗅覚", "Smell Metal");
        rc_ptr->power_desc[rc_ptr->num].min_level = 3;
        rc_ptr->power_desc[rc_ptr->num].cost = 2;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 12;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::SMELL_MET);
    }

    if (creature_ptr->muta.has(MUTA::SMELL_MON)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("敵臭嗅覚", "Smell Monsters");
        rc_ptr->power_desc[rc_ptr->num].min_level = 5;
        rc_ptr->power_desc[rc_ptr->num].cost = 4;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::SMELL_MON);
    }

    if (creature_ptr->muta.has(MUTA::BLINK)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("ショート・テレポート", "Blink");
        rc_ptr->power_desc[rc_ptr->num].min_level = 3;
        rc_ptr->power_desc[rc_ptr->num].cost = 3;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 12;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::BLINK);
    }

    if (creature_ptr->muta.has(MUTA::EAT_ROCK)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("岩食い", "Eat Rock");
        rc_ptr->power_desc[rc_ptr->num].min_level = 8;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 18;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::EAT_ROCK);
    }

    if (creature_ptr->muta.has(MUTA::SWAP_POS)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("位置交換", "Swap Position");
        rc_ptr->power_desc[rc_ptr->num].min_level = 15;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_DEX;
        rc_ptr->power_desc[rc_ptr->num].fail = 16;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::SWAP_POS);
    }

    if (creature_ptr->muta.has(MUTA::SHRIEK)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("叫び", "Shriek");
        rc_ptr->power_desc[rc_ptr->num].min_level = 20;
        rc_ptr->power_desc[rc_ptr->num].cost = 14;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 16;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::SHRIEK);
    }

    if (creature_ptr->muta.has(MUTA::ILLUMINE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("照明", "Illuminate");
        rc_ptr->power_desc[rc_ptr->num].min_level = 3;
        rc_ptr->power_desc[rc_ptr->num].cost = 2;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 10;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::ILLUMINE);
    }

    if (creature_ptr->muta.has(MUTA::DET_CURSE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("呪い感知", "Detect Curses");
        rc_ptr->power_desc[rc_ptr->num].min_level = 7;
        rc_ptr->power_desc[rc_ptr->num].cost = 14;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::DET_CURSE);
    }

    if (creature_ptr->muta.has(MUTA::BERSERK)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("狂戦士化", "Berserk");
        rc_ptr->power_desc[rc_ptr->num].min_level = 8;
        rc_ptr->power_desc[rc_ptr->num].cost = 8;
        rc_ptr->power_desc[rc_ptr->num].stat = A_STR;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::BERSERK);
    }

    if (creature_ptr->muta.has(MUTA::POLYMORPH)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("変身", "Polymorph");
        rc_ptr->power_desc[rc_ptr->num].min_level = 18;
        rc_ptr->power_desc[rc_ptr->num].cost = 20;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 18;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::POLYMORPH);
    }

    if (creature_ptr->muta.has(MUTA::MIDAS_TCH)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("ミダスの手", "Midas Touch");
        rc_ptr->power_desc[rc_ptr->num].min_level = 10;
        rc_ptr->power_desc[rc_ptr->num].cost = 5;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 12;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::MIDAS_TCH);
    }

    if (creature_ptr->muta.has(MUTA::GROW_MOLD)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("カビ発生", "Grow Mold");
        rc_ptr->power_desc[rc_ptr->num].min_level = 1;
        rc_ptr->power_desc[rc_ptr->num].cost = 6;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::GROW_MOLD);
    }

    if (creature_ptr->muta.has(MUTA::RESIST)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("エレメント耐性", "Resist Elements");
        rc_ptr->power_desc[rc_ptr->num].min_level = 10;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 12;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::RESIST);
    }

    if (creature_ptr->muta.has(MUTA::EARTHQUAKE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("地震", "Earthquake");
        rc_ptr->power_desc[rc_ptr->num].min_level = 12;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_STR;
        rc_ptr->power_desc[rc_ptr->num].fail = 16;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::EARTHQUAKE);
    }

    if (creature_ptr->muta.has(MUTA::EAT_MAGIC)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("魔力食い", "Eat Magic");
        rc_ptr->power_desc[rc_ptr->num].min_level = 17;
        rc_ptr->power_desc[rc_ptr->num].cost = 1;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::EAT_MAGIC);
    }

    if (creature_ptr->muta.has(MUTA::WEIGH_MAG)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("魔力感知", "Weigh Magic");
        rc_ptr->power_desc[rc_ptr->num].min_level = 6;
        rc_ptr->power_desc[rc_ptr->num].cost = 6;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 10;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::WEIGH_MAG);
    }

    if (creature_ptr->muta.has(MUTA::STERILITY)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("増殖阻止", "Sterilize");
        rc_ptr->power_desc[rc_ptr->num].min_level = 12;
        rc_ptr->power_desc[rc_ptr->num].cost = 23;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CHR;
        rc_ptr->power_desc[rc_ptr->num].fail = 15;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::STERILITY);
    }

    if (creature_ptr->muta.has(MUTA::HIT_AND_AWAY)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("ヒット＆アウェイ", "Panic Hit");
        rc_ptr->power_desc[rc_ptr->num].min_level = 10;
        rc_ptr->power_desc[rc_ptr->num].cost = 12;
        rc_ptr->power_desc[rc_ptr->num].stat = A_DEX;
        rc_ptr->power_desc[rc_ptr->num].fail = 14;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::HIT_AND_AWAY);
    }

    if (creature_ptr->muta.has(MUTA::DAZZLE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("眩惑", "Dazzle");
        rc_ptr->power_desc[rc_ptr->num].min_level = 7;
        rc_ptr->power_desc[rc_ptr->num].cost = 15;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CHR;
        rc_ptr->power_desc[rc_ptr->num].fail = 8;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::DAZZLE);
    }

    if (creature_ptr->muta.has(MUTA::LASER_EYE)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("レーザー・アイ", "Laser Eye");
        rc_ptr->power_desc[rc_ptr->num].min_level = 7;
        rc_ptr->power_desc[rc_ptr->num].cost = 10;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 9;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::LASER_EYE);
    }

    if (creature_ptr->muta.has(MUTA::RECALL)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("帰還", "Recall");
        rc_ptr->power_desc[rc_ptr->num].min_level = 17;
        rc_ptr->power_desc[rc_ptr->num].cost = 50;
        rc_ptr->power_desc[rc_ptr->num].stat = A_INT;
        rc_ptr->power_desc[rc_ptr->num].fail = 16;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::RECALL);
    }

    if (creature_ptr->muta.has(MUTA::BANISH)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("邪悪消滅", "Banish Evil");
        rc_ptr->power_desc[rc_ptr->num].min_level = 25;
        rc_ptr->power_desc[rc_ptr->num].cost = 25;
        rc_ptr->power_desc[rc_ptr->num].stat = A_WIS;
        rc_ptr->power_desc[rc_ptr->num].fail = 18;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::BANISH);
    }

    if (creature_ptr->muta.has(MUTA::COLD_TOUCH)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("凍結の手", "Cold Touch");
        rc_ptr->power_desc[rc_ptr->num].min_level = 2;
        rc_ptr->power_desc[rc_ptr->num].cost = 2;
        rc_ptr->power_desc[rc_ptr->num].stat = A_CON;
        rc_ptr->power_desc[rc_ptr->num].fail = 11;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::COLD_TOUCH);
    }

    if (creature_ptr->muta.has(MUTA::LAUNCHER)) {
        rc_ptr->power_desc[rc_ptr->num].racial_name = _("アイテム投げ", "Throw Object");
        rc_ptr->power_desc[rc_ptr->num].min_level = 1;
        rc_ptr->power_desc[rc_ptr->num].cost = rc_ptr->lvl;
        rc_ptr->power_desc[rc_ptr->num].stat = A_STR;
        rc_ptr->power_desc[rc_ptr->num].fail = 6;
        rc_ptr->power_desc[rc_ptr->num++].number = static_cast<int>(MUTA::LAUNCHER);
    }
}
