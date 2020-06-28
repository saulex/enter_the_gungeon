#ifndef _ETG_CONFIG_HPP_
#define _ETG_CONFIG_HPP_

#include "cocos2d.h"
#include <map>
#include <set>

using namespace cocos2d;

namespace etg {

enum class TAG {
    player_node,
    player_body,
    player_anm,
    player_hurt_act,

    enemy_node,
    enemy_body,
    enemy_anm,
    enemy_hurt_anm,

    bullet_node,
    wall_node,
    map_node,
    camera_node
};

// contact mask
enum class C_MASK {
    character = 0x01, // 0001
    wall = 0x02, // 0010
    bullet = 0x04, // 0100
    player = 0x08, // 1000
    enemy = 0x10, // 0001,0000
    placeholder1 = 0x20, // 0010,0000
    placeholder2 = 0x40, // 0100,0000
    placeholder3 = 0x80, // 1000,0000
    all = 0xff
};

class FilePath {
public:
    static constexpr const char
        *player_bullet
        = "bullet/player_bullet.png",
        *enemy_bullet = "bullet/enemy_bullet.png",
        *fake_door = "door/door.png";
};

// correspond to base_room.tmx
const static std::set<int> base_room_walkable_gid = {
    1, 2, 3, 4,
    17, 18, 19, 20,
    98, 99, 100,
    114, 115, 116
};

const static Vec2 DEFAULT_MOVE_SPEED = { 1, 1 };
const static Vec2 SPEED_MOVE_ENEMY = { 40, 40 };
// 先生成固定数目的怪物，再按概率生成其他怪物
const static float ENEMY_GENERATE_PROB = 0.4f;
const static int ENEMY_GENERATE_LEAST = 3;
const static std::vector<int> ENEMY_GENERATE_POS = { 3, 3 };
// BULLET
const static Vec2 SPEED_BULLET_PLAYER = { 200, 200 };
const static Vec2 SPEED_BULLET_ENEMY = { 100, 100 };
const static int DAMAGE_PLAYER_BULLET = 1;
const static int DAMAGE_ENEMY_BULLET = 1;
// HP
const static int HP_LIMIT_PLAYER = 6;
const static int HP_LIMIT_ENEMY = 4;
// SHOT
const static float SHOT_INTERVAL_ENEMY = 3.0f;
const static float SHOT_DELAY_ENEMY = 0.5f; // shot delay in one round
const static float SHOT_INTERVAL_PLAYER = 0.4f;
const static int SHOT_NUMBER_ENEMY = 3;
// DUNGEON SIZE
const static int DUNGEON_SIZE_WIDTH = 3;
const static int DUNGEON_SIZE_HEIGHT = 3;
// MAP TYPE
enum class MAP_TYPE {
    NORMAL,
    BOSS
};
}

#endif //_ETG_CONFIG_HPP_