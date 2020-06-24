#ifndef _COCOS_MY_UTILS_
#define _COCOS_MY_UTILS_

#include <list>
#include <set>
#include <string>

#include "cocos2d.h"

namespace myutl {

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

std::list<unsigned long> range(unsigned long begin, unsigned long end);

std::list<unsigned long> range(unsigned long end);

template <class T>
bool contains(const std::set<T>& super, const std::set<T>& sub);

void move_by(cocos2d::Node* node, const cocos2d::Vec2& delta);

cocos2d::Vec2 dot(const cocos2d::Vec2& a, const cocos2d::Vec2& b);

std::string v2s(const cocos2d::Vec2& a);

cocos2d::Size v2size(const cocos2d::Vec2& a);

bool veq(const cocos2d::Vec2& a, const cocos2d::Vec2& b);

cocos2d::Vec2 d2v(DIR d);

}

#endif //_COCOS_MY_UTILS_
