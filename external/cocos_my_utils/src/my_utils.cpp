#ifndef COCOS_MY_UTILS_HPP
#define COCOS_MY_UTILS_HPP

#include <my_utils.hpp>
#include <stdexcept>

namespace myutl {

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

}

#endif //COCOS_MY_UTILS_HPP