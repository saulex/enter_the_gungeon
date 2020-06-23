#ifndef _ETG_STATUS_HPP_
#define _ETG_STATUS_HPP_

#include <string>

namespace etg {

class Status {

public:
    explicit Status(const bool stoppable,
        const bool has_begin = false,
        const bool has_end = false,
        const std::string& name = "")
        : name(name)
        , stoppable(stoppable)
        , has_begin(has_begin)
        , has_end(has_end)
    {
    }
    std::string name;
    bool stoppable;
    bool has_begin;
    bool has_end;
};

};

#endif //_ETG_STATUS_HPP_