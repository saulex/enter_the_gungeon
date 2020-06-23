#ifndef _ETG_CONFIG_HPP_
#define _ETG_CONFIG_HPP_

#include "cocos2d.h"
#include <map>
#include <set>

using namespace cocos2d;

namespace etg {

enum ORDER // 越小越先绘制
{
    ground,
    base_wall,
    ground_1,
    base_wall_1,
    decoration,
    player,
    other_wall,
    other_wall_1,
    ui
};

enum DIR {
    E, // empty
    U,
    D,
    L,
    R,
    UL,
    UR,
    DL,
    DR,
    DIR_END
};

enum TAG {
    player_body,
    wall_body
};

// correspond to base_room.tmx
const static std::set<int> base_room_walkable_gid = {
    1, 2, 3, 4,
    17, 18, 19, 20,
    98, 99, 100,
    114, 115, 116
};

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

static Vec2 d2v(DIR d)
{
    if (d == DIR::E)
        return { 0, 0 };
    if (d == DIR::U)
        return { 0, 1 };
    if (d == DIR::D)
        return { 0, -1 };
    if (d == DIR::L)
        return { 0, 0 };
    if (d == DIR::R)
        return { 1, 0 };
    if (d == DIR::UL)
        return { -1, 1 };
    if (d == DIR::UR)
        return { 1, 1 };
    if (d == DIR::DL)
        return { -1, -1 };
    if (d == DIR::DR)
        return { 1, -1 };
}

}

#endif //_ETG_CONFIG_HPP_