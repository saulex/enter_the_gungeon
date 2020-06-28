#ifndef COCOS_MY_UTILS_HPP
#define COCOS_MY_UTILS_HPP

#include <math.h>
#include <my_utils.hpp>
#include <stdexcept>

namespace myutl {

void mylog(const std::string& s, bool flag)
{
    if (flag)
        cocos2d::log(s.c_str());
}

void mylog(const char* s, bool flag)
{
    if (flag)
        cocos2d::log(s);
}

std::list<unsigned long> range(unsigned long begin, unsigned long end)
{
    if (begin >= end) {
        throw std::invalid_argument("begin should less than end");
    }
    std::list<unsigned long> a;
    while (begin < end) {
        a.push_back(begin);
        begin += 1;
    }
    return a;
}

std::list<unsigned long> range(unsigned long end)
{
    return range(0, end);
}

template <class T>
bool contains(const std::set<T>& super, const std::set<T>& sub)
{
    for (const auto& ele_in_sub : sub) {
        if (super.find(ele_in_sub) != super.end()) {
            return false;
        }
    }
    return true;
}

void move_by(cocos2d::Node* node, const cocos2d::Vec2& delta)
{
    node->setPosition(node->getPosition() + delta);
}

cocos2d::Vec2 dot(const cocos2d::Vec2& a, const cocos2d::Vec2& b)
{
    return { a.x * b.x, a.y * b.y };
}

std::string v2s(const cocos2d::Vec2& a)
{
    return "(" + std::to_string(a.x) + ","
        + std::to_string(a.y) + ")";
}

cocos2d::Vec2 v_size()
{
    return cocos2d::Director::getInstance()->getVisibleSize();
}

bool veq(const cocos2d::Vec2& a, const cocos2d::Vec2& b)
{
    const float e = 1e-5;
    return abs(a.x - b.x) < e && abs(a.y - b.y) < e;
}

cocos2d::Vec2 d2v(DIR d)
{
    if (d == DIR::E)
        return { 0, 0 };
    if (d == DIR::U)
        return { 0, 1 };
    if (d == DIR::D)
        return { 0, -1 };
    if (d == DIR::L)
        return { -1, 0 };
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

    return { 0, 0 };
}

DIR v2d(const cocos2d::Vec2& v)
{
    if (veq(v, { 0, 0 }))
        return DIR::E;
    if (veq(v, { 0, 1 }))
        return DIR::U;
    if (veq(v, { 0, -1 }))
        return DIR::D;
    if (veq(v, { -1, 0 }))
        return DIR::L;
    if (veq(v, { 1, 0 }))
        return DIR::R;
    if (veq(v, { -1, 1 }))
        return DIR::UL;
    if (veq(v, { 1, 1 }))
        return DIR::UR;
    if (veq(v, { -1, -1 }))
        return DIR::DL;
    if (veq(v, { 1, -1 }))
        return DIR::DR;
    return DIR::E;
}

std::vector<DIR> all4dirs()
{
    return { DIR::U, DIR::D, DIR::L, DIR::R };
}

struct AnimInfo animation_generator(
    const std::string& dir,
    const std::string& file,
    float delay, int n,
    const std::string& prefix,
    const std::string& suffix)
{
    auto cache = cocos2d::SpriteFrameCache::getInstance();
    std::string plist = dir + file + ".plist";
    cache->addSpriteFramesWithFile(plist);
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i < n + 1; i++) {
        auto name = prefix + file + std::to_string(i) + suffix;
        auto frame = cache->getSpriteFrameByName(name);
        frames.pushBack(frame);
    }
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames);
    animation->setRestoreOriginalFrame(true);
    animation->setDelayPerUnit(delay);

    return { animation, frames };
}

}

#endif //COCOS_MY_UTILS_HPP