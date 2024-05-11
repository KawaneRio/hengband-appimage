#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/*! nestのID定義 /  Nest types code */
enum nest_type : int {
    NEST_TYPE_CLONE = 0,
    NEST_TYPE_JELLY = 1,
    NEST_TYPE_SYMBOL_GOOD = 2,
    NEST_TYPE_SYMBOL_EVIL = 3,
    NEST_TYPE_MIMIC = 4,
    NEST_TYPE_LOVECRAFTIAN = 5,
    NEST_TYPE_KENNEL = 6,
    NEST_TYPE_ANIMAL = 7,
    NEST_TYPE_CHAPEL = 8,
    NEST_TYPE_UNDEAD = 9,
};

/*! pitのID定義 / Pit types code */
enum pit_type : int {
    PIT_TYPE_ORC = 0,
    PIT_TYPE_TROLL = 1,
    PIT_TYPE_GIANT = 2,
    PIT_TYPE_LOVECRAFTIAN = 3,
    PIT_TYPE_SYMBOL_GOOD = 4,
    PIT_TYPE_SYMBOL_EVIL = 5,
    PIT_TYPE_CHAPEL = 6,
    PIT_TYPE_DRAGON = 7,
    PIT_TYPE_DEMON = 8,
    PIT_TYPE_DARK_ELF = 9,
};

/*! pit/nest型情報の構造体定義 */
enum class MonsterRaceId : short;
class PlayerType;
struct nest_pit_type {
    std::string name; //<! 部屋名
    std::function<bool(PlayerType *, MonsterRaceId)> hook_func; //<! モンスターフィルタ関数
    std::optional<std::function<void(PlayerType *)>> prep_func; //<! 能力フィルタ関数
    int level; //<! 相当階
    int chance; //!< 生成確率
};

/*! デバッグ時にnestのモンスター情報を確認するための構造体 / A struct for nest monster information with cheat_hear */
struct nest_mon_info_type {
    MonsterRaceId r_idx; //!< モンスター種族ID
    bool used = false; //!< 既に選んだかどうか
};

struct dun_data_type;
std::string pit_subtype_string(int type, bool nest);
int pick_vault_type(const std::vector<nest_pit_type> &np_types, uint16_t allow_flag_mask, int dun_level);
bool build_type5(PlayerType *player_ptr, dun_data_type *dd_ptr);
