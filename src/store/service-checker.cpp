﻿#include "store/service-checker.h"
#include "monster-race/monster-race.h"
#include "monster-race/race-flags3.h"
#include "object-enchant/tr-types.h"
#include "object/object-flags.h"
#include "object/object-value.h"
#include "store/store-util.h"
#include "sv-definition/sv-potion-types.h"
#include "sv-definition/sv-rod-types.h"
#include "sv-definition/sv-scroll-types.h"
#include "sv-definition/sv-weapon-types.h"
#include "system/monster-race-definition.h"
#include "system/object-type-definition.h"
#include "system/player-type-definition.h"
#include "util/bit-flags-calculator.h"
#include "util/string-processor.h"

/*!
 * @brief オブジェクトが祝福されているかの判定を返す /
 * @param o_ptr 判定したいオブジェクト構造体の参照ポインタ
 * @return アイテムが祝福されたアイテムならばTRUEを返す
 */
static bool is_blessed_item(const object_type *o_ptr)
{
    auto flgs = object_flags(o_ptr);
    return flgs.has(TR_BLESSED);
}

static bool check_store_general(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_ROD:
        return (o_ptr->sval == SV_ROD_PESTICIDE);
    case ItemPrimaryType::TV_POTION:
        return (o_ptr->sval == SV_POTION_WATER);
    case ItemPrimaryType::TV_WHISTLE:
    case ItemPrimaryType::TV_FOOD:
    case ItemPrimaryType::TV_LITE:
    case ItemPrimaryType::TV_FLASK:
    case ItemPrimaryType::TV_SPIKE:
    case ItemPrimaryType::TV_SHOT:
    case ItemPrimaryType::TV_ARROW:
    case ItemPrimaryType::TV_BOLT:
    case ItemPrimaryType::TV_DIGGING:
    case ItemPrimaryType::TV_CLOAK:
    case ItemPrimaryType::TV_BOTTLE:
    case ItemPrimaryType::TV_FIGURINE:
    case ItemPrimaryType::TV_STATUE:
    case ItemPrimaryType::TV_CAPTURE:
    case ItemPrimaryType::TV_CARD:
        return true;
    default:
        return false;
    }
}

static bool check_store_armoury(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_BOOTS:
    case ItemPrimaryType::TV_GLOVES:
    case ItemPrimaryType::TV_CROWN:
    case ItemPrimaryType::TV_HELM:
    case ItemPrimaryType::TV_SHIELD:
    case ItemPrimaryType::TV_CLOAK:
    case ItemPrimaryType::TV_SOFT_ARMOR:
    case ItemPrimaryType::TV_HARD_ARMOR:
    case ItemPrimaryType::TV_DRAG_ARMOR:
        return true;
    default:
        return false;
    }
}

static bool check_store_weapon(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_SHOT:
    case ItemPrimaryType::TV_BOLT:
    case ItemPrimaryType::TV_ARROW:
    case ItemPrimaryType::TV_BOW:
    case ItemPrimaryType::TV_DIGGING:
    case ItemPrimaryType::TV_POLEARM:
    case ItemPrimaryType::TV_SWORD:
    case ItemPrimaryType::TV_HISSATSU_BOOK:
        return true;
    case ItemPrimaryType::TV_HAFTED:
        return o_ptr->sval != SV_WIZSTAFF;
    default:
        return false;
    }
}

static bool check_store_temple(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_LIFE_BOOK:
    case ItemPrimaryType::TV_CRUSADE_BOOK:
    case ItemPrimaryType::TV_SCROLL:
    case ItemPrimaryType::TV_POTION:
    case ItemPrimaryType::TV_HAFTED:
        return true;
    case ItemPrimaryType::TV_FIGURINE:
    case ItemPrimaryType::TV_STATUE: {
        monster_race *r_ptr = &r_info[o_ptr->pval];
        if (!(r_ptr->flags3 & RF3_EVIL))
            if (((r_ptr->flags3 & RF3_GOOD) != 0) || ((r_ptr->flags3 & RF3_ANIMAL) != 0) || (angband_strchr("?!", r_ptr->d_char) != nullptr))
                return true;
    }
        /* Fall through */
    case ItemPrimaryType::TV_POLEARM:
    case ItemPrimaryType::TV_SWORD:
        if (is_blessed_item(o_ptr))
            return true;

        /* Fall through */
    default:
        return false;
    }
}

static bool check_store_alchemist(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_SCROLL:
    case ItemPrimaryType::TV_POTION:
        return true;
    default:
        return false;
    }
}

static bool check_store_magic(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_SORCERY_BOOK:
    case ItemPrimaryType::TV_NATURE_BOOK:
    case ItemPrimaryType::TV_CHAOS_BOOK:
    case ItemPrimaryType::TV_DEATH_BOOK:
    case ItemPrimaryType::TV_TRUMP_BOOK:
    case ItemPrimaryType::TV_ARCANE_BOOK:
    case ItemPrimaryType::TV_CRAFT_BOOK:
    case ItemPrimaryType::TV_DEMON_BOOK:
    case ItemPrimaryType::TV_MUSIC_BOOK:
    case ItemPrimaryType::TV_HEX_BOOK:
    case ItemPrimaryType::TV_AMULET:
    case ItemPrimaryType::TV_RING:
    case ItemPrimaryType::TV_STAFF:
    case ItemPrimaryType::TV_WAND:
    case ItemPrimaryType::TV_ROD:
    case ItemPrimaryType::TV_SCROLL:
    case ItemPrimaryType::TV_POTION:
    case ItemPrimaryType::TV_FIGURINE:
        return true;
    case ItemPrimaryType::TV_HAFTED:
        return o_ptr->sval == SV_WIZSTAFF;
    default:
        return false;
    }
}

static bool check_store_book(const object_type *o_ptr)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_SORCERY_BOOK:
    case ItemPrimaryType::TV_NATURE_BOOK:
    case ItemPrimaryType::TV_CHAOS_BOOK:
    case ItemPrimaryType::TV_DEATH_BOOK:
    case ItemPrimaryType::TV_LIFE_BOOK:
    case ItemPrimaryType::TV_TRUMP_BOOK:
    case ItemPrimaryType::TV_ARCANE_BOOK:
    case ItemPrimaryType::TV_CRAFT_BOOK:
    case ItemPrimaryType::TV_DEMON_BOOK:
    case ItemPrimaryType::TV_CRUSADE_BOOK:
    case ItemPrimaryType::TV_MUSIC_BOOK:
    case ItemPrimaryType::TV_HEX_BOOK:
        return true;
    default:
        return false;
    }
}

static bool switch_store_check(const object_type *o_ptr)
{
    switch (cur_store_num) {
    case StoreSaleType::GENERAL:
        return check_store_general(o_ptr);
    case StoreSaleType::ARMOURY:
        return check_store_armoury(o_ptr);
    case StoreSaleType::WEAPON:
        return check_store_weapon(o_ptr);
    case StoreSaleType::TEMPLE:
        return check_store_temple(o_ptr);
    case StoreSaleType::ALCHEMIST:
        return check_store_alchemist(o_ptr);
    case StoreSaleType::MAGIC:
        return check_store_magic(o_ptr);
    case StoreSaleType::BOOK:
        return check_store_book(o_ptr);
    default:
        return true;
    }
}

/*!
 * @brief オブジェクトが所定の店舗で引き取れるかどうかを返す /
 * Determine if the current store will purchase the given item
 * @param o_ptr 判定したいオブジェクト構造体の参照ポインタ
 * @return アイテムが買い取れるならばTRUEを返す
 * @note
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
bool store_will_buy(player_type *, const object_type *o_ptr)
{
    if ((cur_store_num == StoreSaleType::HOME) || (cur_store_num == StoreSaleType::MUSEUM))
        return true;

    if (!switch_store_check(o_ptr))
        return false;

    return object_value(o_ptr) > 0;
}

static int mass_lite_produce(const PRICE cost)
{
    int size = 1;
    if (cost <= 5L)
        size += damroll(3, 5);

    if (cost <= 20L)
        size += damroll(3, 5);

    if (cost <= 50L)
        size += damroll(2, 2);

    return size;
}

static int mass_scroll_produce(object_type *o_ptr, const PRICE cost)
{
    int size = 1;
    if (cost <= 60L)
        size += damroll(3, 5);

    if (cost <= 240L)
        size += damroll(1, 5);

    if (o_ptr->sval == SV_SCROLL_STAR_IDENTIFY)
        size += damroll(3, 5);

    if (o_ptr->sval == SV_SCROLL_STAR_REMOVE_CURSE)
        size += damroll(1, 4);

    return size;
}

static int mass_book_produce(const PRICE cost)
{
    int size = 1;
    if (cost <= 50L)
        size += damroll(2, 3);

    if (cost <= 500L)
        size += damroll(1, 3);

    return size;
}

static int mass_equipment_produce(object_type *o_ptr, const PRICE cost)
{
    int size = 1;
    if (o_ptr->is_artifact() || o_ptr->is_ego())
        return size;

    if (cost <= 10L)
        size += damroll(3, 5);

    if (cost <= 100L)
        size += damroll(3, 5);

    return size;
}

static int mass_arrow_produce(const PRICE cost)
{
    int size = 1;
    if (cost <= 5L)
        size += damroll(5, 5);

    if (cost <= 50L)
        size += damroll(5, 5);

    if (cost <= 500L)
        size += damroll(5, 5);

    return size;
}

static int mass_figurine_produce(const PRICE cost)
{
    int size = 1;
    if (cost <= 100L)
        size += damroll(2, 2);

    if (cost <= 1000L)
        size += damroll(2, 2);

    return size;
}

static int mass_magic_produce(const PRICE cost)
{
    int size = 1;
    if ((cur_store_num != StoreSaleType::BLACK) || !one_in_(3))
        return size;

    if (cost < 1601L)
        size += damroll(1, 5);
    else if (cost < 3201L)
        size += damroll(1, 3);

    return size;
}

static int switch_mass_production(object_type *o_ptr, const PRICE cost)
{
    switch (o_ptr->tval) {
    case ItemPrimaryType::TV_FOOD:
    case ItemPrimaryType::TV_FLASK:
    case ItemPrimaryType::TV_LITE:
        return mass_lite_produce(cost);
    case ItemPrimaryType::TV_POTION:
    case ItemPrimaryType::TV_SCROLL:
        return mass_scroll_produce(o_ptr, cost);
    case ItemPrimaryType::TV_LIFE_BOOK:
    case ItemPrimaryType::TV_SORCERY_BOOK:
    case ItemPrimaryType::TV_NATURE_BOOK:
    case ItemPrimaryType::TV_CHAOS_BOOK:
    case ItemPrimaryType::TV_DEATH_BOOK:
    case ItemPrimaryType::TV_TRUMP_BOOK:
    case ItemPrimaryType::TV_ARCANE_BOOK:
    case ItemPrimaryType::TV_CRAFT_BOOK:
    case ItemPrimaryType::TV_DEMON_BOOK:
    case ItemPrimaryType::TV_CRUSADE_BOOK:
    case ItemPrimaryType::TV_MUSIC_BOOK:
    case ItemPrimaryType::TV_HISSATSU_BOOK:
    case ItemPrimaryType::TV_HEX_BOOK:
        return mass_book_produce(cost);
    case ItemPrimaryType::TV_SOFT_ARMOR:
    case ItemPrimaryType::TV_HARD_ARMOR:
    case ItemPrimaryType::TV_SHIELD:
    case ItemPrimaryType::TV_GLOVES:
    case ItemPrimaryType::TV_BOOTS:
    case ItemPrimaryType::TV_CLOAK:
    case ItemPrimaryType::TV_HELM:
    case ItemPrimaryType::TV_CROWN:
    case ItemPrimaryType::TV_SWORD:
    case ItemPrimaryType::TV_POLEARM:
    case ItemPrimaryType::TV_HAFTED:
    case ItemPrimaryType::TV_DIGGING:
    case ItemPrimaryType::TV_BOW:
        return mass_equipment_produce(o_ptr, cost);
    case ItemPrimaryType::TV_SPIKE:
    case ItemPrimaryType::TV_SHOT:
    case ItemPrimaryType::TV_ARROW:
    case ItemPrimaryType::TV_BOLT:
        return mass_arrow_produce(cost);
    case ItemPrimaryType::TV_FIGURINE:
        return mass_figurine_produce(cost);
    case ItemPrimaryType::TV_CAPTURE:
    case ItemPrimaryType::TV_STATUE:
    case ItemPrimaryType::TV_CARD:
        return 1;
    case ItemPrimaryType::TV_ROD:
    case ItemPrimaryType::TV_WAND:
    case ItemPrimaryType::TV_STAFF:
        return mass_magic_produce(cost);
    default:
        return 1;
    }
}

static byte decide_discount_rate(const PRICE cost)
{
    if (cost < 5)
        return 0;
    
    if (one_in_(25))
        return 25;
    
    if (one_in_(150))
        return 50;
    
    if (one_in_(300))
        return 75;
    
    if (one_in_(500))
        return 90;

    return 0;
}

/*!
 * @brief 安価な消耗品の販売数を増やし、低確率で割引にする /
 * Certain "cheap" objects should be created in "piles"
 * @param o_ptr 店舗に並べるオブジェクト構造体の参照ポインタ
 * @details
 * <pre>
 * Some objects can be sold at a "discount" (in small piles)
 * </pre>
 */
void mass_produce(player_type *, object_type *o_ptr)
{
    const PRICE cost = object_value(o_ptr);
    int size = switch_mass_production(o_ptr, cost);
    auto discount = decide_discount_rate(cost);
    if (o_ptr->art_name)
        discount = 0;

    o_ptr->discount = discount;
    o_ptr->number = size - (size * discount / 100);
    if ((o_ptr->tval == ItemPrimaryType::TV_ROD) || (o_ptr->tval == ItemPrimaryType::TV_WAND))
        o_ptr->pval *= (PARAMETER_VALUE)o_ptr->number;
}
