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

    bullet_node,
    wall_node,
    map_node,
    camera_node
};

// contact mask
enum class C_MASK {
    character = 0x01, // 0001
    wall = 0x02, // 0010
    bullet = 0x04 // 0100
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
const static Vec2 SPEED_BULLET_PLAYER = { 200, 200 };

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