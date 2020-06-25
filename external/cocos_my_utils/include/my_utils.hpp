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
    E = 0x00, // 0000 empty
    U = 0x01, // 0001
    D = 0x02, // 0010
    L = 0x04, // 0100
    R = 0x08, // 1000
    UL = 0x05, // 0101
    UR = 0x09, // 1001
    DL = 0x06, // 0110
    DR = 0x0A // 1010
};

void mylog(const std::string& s, bool flag = true);

void mylog(const char* s, bool flag = true);

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

struct AnimInfo {
    cocos2d::Animation* animation;
    cocos2d::Vector<cocos2d::SpriteFrame*> sfs;
};

struct AnimInfo animation_generator(
    const std::string& dir,
    const std::string& file,
    float delay, int n,
    const std::string& prefix = "",
    const std::string& suffix = ".png");

}

#endif //_COCOS_MY_UTILS_
