#include <list>
#include <set>

namespace myutl {

std::list<unsigned long> range(unsigned long begin, unsigned long end);

std::list<unsigned long> range(unsigned long end);

template <class T>
bool contains(const std::set<T>& super, const std::set<T>& sub);
}