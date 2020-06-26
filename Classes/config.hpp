#ifndef _ETG_CONFIG_HPP_
#define _ETG_CONFIG_HPP_

#include "cocos2d.h"
#include <map>
#include <set>

using namespace cocos2d;

namespace etg {

enum class TAG {
    player_node,
    player_anm,

    enemy_node,
    enemy_anm,

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
    static constexpr const char*
        player_bullet
        = "bullet/player_bullet.png";
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
const static Vec2 SPEED_BULLET_PLAYER = { 200, 200 };
// 先生成固定数目的怪物，再按概率生成其他怪物
const static float ENEMY_GENERATE_PROB = 0.4f;
const static int ENEMY_GENERATE_LEAST = 3;
const static std::vector<int> ENEMY_GENERATE_POS = { 3, 3 };

//const static std::map<DIR, Vec2> d2v = {
//    { DIR::E, { 0, 0 } },
//    { DIR::U, { 0, 1 } },
//    { DIR::D, { 0, -1 } },
//    { DIR::L, { -1, 0 } },
//    { DIR::R, { 1, 0 } },
//    { DIR::UL, { -1, 1 } },
//    { DIR::UR, { 1, 1 } },
//    { DIR::DL, { -1, -1 } },
//    { DIR::DR, { 1, -1 } }
//};

}

#endif //_ETG_CONFIG_HPP_