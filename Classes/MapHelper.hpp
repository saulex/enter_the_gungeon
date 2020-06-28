#ifndef _ETG_MAP_HELPER_HPP_
#define _ETG_MAP_HELPER_HPP_

#include <cocos2d.h>

#include <map>
#include <vector>

#include <GungeonScene.h>
#include <config.hpp>

namespace etg {

class MapStatus {
public:
    MapStatus(
        MAP_TYPE type = MAP_TYPE::NORMAL,
        bool is_clear = false,
        bool is_null = false)
        : type(type)
        , is_clear(is_clear)
        , is_null(is_null)
    {
        for (DIR dir : { DIR::U, DIR::D, DIR::L, DIR::R })
            door_open[dir] = false;
        pos = { -1, -1 };
    };
    MAP_TYPE type;
    bool is_clear, is_null;
    std::map<DIR, bool> door_open;
    std::vector<int> pos; // (height, width)
};

class HeroStatus {
public:
    HeroStatus(
        int hp = HP_LIMIT_PLAYER,
        int hp_limit = HP_LIMIT_PLAYER,
        DIR come_from = DIR::E)
        : hp(hp)
        , hp_limit(hp_limit)
        , come_from(come_from) {};
    int hp, hp_limit;
    DIR come_from; // possible value {E, U, D, L, R}
};

class MapHelper {

protected:
    MapHelper();
    ~MapHelper();

public:
    static MapHelper* get_instance();
    MapStatus get_cur_map_info();
    HeroStatus get_hero_status();


    void cur_map_clear();
    void go_to_map_on(DIR dir, int hp, int hp_limit);

protected:
    static MapHelper* instance;

    HeroStatus hero_status;
    std::vector<int> cur_map_pos; // (height, width)
    std::vector<std::vector<MapStatus>> map_info; // (height, width)

    const int width, height;
    void init_dungeon();
};

};

#endif //_ETG_MAP_HELPER_HPP_