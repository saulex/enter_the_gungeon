#include <MapHelper.hpp>

#include <my_utils.hpp>
using namespace cocos2d;

namespace etg {

MapHelper* MapHelper::instance = nullptr;

MapHelper::MapHelper()
    : width(DUNGEON_SIZE_WIDTH)
    , height(DUNGEON_SIZE_HEIGHT)
{
    init_dungeon();
    cur_map_pos = { 0, 0 };
    hero_status = HeroStatus();
}

MapHelper::~MapHelper()
{
    delete instance;
}

MapHelper* MapHelper::get_instance()
{
    if (!instance) {
        instance = new MapHelper();
    }
    return instance;
}

MapStatus MapHelper::get_cur_map_info()
{
    return map_info[cur_map_pos[0]][cur_map_pos[1]];
}

void MapHelper::cur_map_clear()
{
    map_info[cur_map_pos[0]][cur_map_pos[1]].is_clear = true;
}

void MapHelper::go_to_map_on(DIR dir, int hp, int hp_limit)
{
    // get the neighbor
    auto v = d2v(dir);
    auto dw = int(v.x);
    auto dh = int(v.y);

    auto h = cur_map_pos[0];
    auto w = cur_map_pos[1];

    if (dw + w < 0 || dw + w >= width || dh + h < 0 || dh + h >= height)
        return;
    if (!map_info[h + dh][w + dw].is_null) {
        // ok, we should move to next map
        // reset cur map info
        cur_map_pos = { h + dh, w + dw };
        // update hero info
        hero_status.hp = hp;
        hero_status.hp_limit = hp_limit;
        hero_status.come_from = v2d(-d2v(dir));
        // TODO change scene
    }
}

void MapHelper::init_dungeon()
{
    // init vector, all null map
    map_info.resize(height);
    for (int i : range(height)) {
        map_info[i].resize(width);
    }
    for (int h : range(height)) {
        for (int w : range(width)) {
            map_info[h][w] = MapStatus(
                MAP_TYPE::NORMAL,
                false,
                true); // IS NULL !
            map_info[h][w].pos = { h, w };
        }
    }
    /*
     * ^ y (height)
     * |
     * map_info[2]: map, map, map, map
     * map_info[1]: map, map, map, map
     * map_info[0]: map, map, map, map --> x (width)
     */
    // init in lower left corner
    map_info[0][0] = MapStatus(MAP_TYPE::NORMAL, true);
    // boss in upper right corner
    map_info[height - 1][width - 1] = MapStatus(MAP_TYPE::BOSS, false);
    // center height is full of monster
    auto ch = int(height / 2);
    for (int w : range(width)) {
        map_info[ch][w] = MapStatus(MAP_TYPE::NORMAL, false);
    }
    // random init
    // TODO Ugly Design. Only reasonable when height = 3
    for (int w : range(width)) {
        for (int h : range(height)) {
            if (map_info[h][w].is_null) {
                if (RandomHelper::random_real(0.0f, 1.0f) < 0.5) {
                    map_info[h][w] = MapStatus(
                        MAP_TYPE::NORMAL,
                        false,
                        false);
                }
            }
        }
    }
    // set doors
    for (int w : range(width)) {
        for (int h : range(height)) {
            // only open doors for NOT NULL map
            if (map_info[h][w].is_null)
                continue;
            for (DIR dir : all4dirs()) {
                // get the neighbor
                auto v = d2v(dir);
                auto dw = int(v.x);
                auto dh = int(v.y);
                if (dw + w < 0 || dw + w >= width || dh + h < 0 || dh + h >= height)
                    continue;

                if (!map_info[h + dh][w + dw].is_null) {
                    map_info[h][w].door_open[dir] = true;
                } else {
                    map_info[h][w].door_open[dir] = false;
                }
            }
        }
    }
}
};