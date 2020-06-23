#include <list>
#include <set>
#include <string>

#include "cocos2d.h"

namespace myutl {

std::list<unsigned long> range(unsigned long begin, unsigned long end);

std::list<unsigned long> range(unsigned long end);

template <class T>
bool contains(const std::set<T>& super, const std::set<T>& sub);

void move_by(cocos2d::Node* node, const cocos2d::Vec2& delta);

cocos2d::Vec2 dot(const cocos2d::Vec2& a, const cocos2d::Vec2& b);

std::string v2s(const cocos2d::Vec2& a);

cocos2d::Size v2size(const cocos2d::Vec2& a);

bool veq(const cocos2d::Vec2& a, const cocos2d::Vec2& b);
}