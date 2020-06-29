#include <MapHelper.hpp>

#include <my_utils.hpp>
using namespace cocos2d;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

namespace etg {

MapHelper* MapHelper::instance = nullptr;

MapHelper::MapHelper()
    : width(DUNGEON_SIZE_WIDTH)
    , height(DUNGEON_SIZE_HEIGHT)
{
    init_dungeon();
    cur_map_pos = { 0, 0 };
    hero_status = HeroStatus();
    // TODO ugly design
    // audio
    init_audio();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.2);
    SimpleAudioEngine::getInstance()->playBackgroundMusic(FilePath::background_fight, true);
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

HeroStatus MapHelper::get_hero_status()
{
    return hero_status;
}

std::vector<int> MapHelper::get_cur_map_pos()
{
    return cur_map_pos;
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

    auto d2trans = [&](DIR d) -> Scene* {
        auto& t = SCENE_REPLACE_TIME;
        auto new_scene = GungeonWorld::createScene();
        if (d == DIR::U)
            return TransitionMoveInT::create(t, new_scene);
        if (d == DIR::D)
            return TransitionMoveInB::create(t, new_scene);
        if (d == DIR::L)
            return TransitionMoveInL::create(t, new_scene);
        if (d == DIR::R)
            return TransitionMoveInR::create(t, new_scene);

        return nullptr;
    };

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
        Director::getInstance()->replaceScene(d2trans(dir));
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
    auto& init_room = map_info[0][0];
    init_room.type = MAP_TYPE::NORMAL;
    init_room.is_clear = true;
    init_room.is_null = false;
    // boss in upper right corner
    auto& boss_room = map_info[height - 1][width - 1];
    boss_room.type = MAP_TYPE::BOSS;
    boss_room.is_clear = false;
    boss_room.is_null = false;
    // center height is full of monster
    auto ch = int(height / 2);
    for (int w : range(width)) {
        map_info[ch][w].type = MAP_TYPE::NORMAL;
        map_info[ch][w].is_clear = false;
        map_info[ch][w].is_null = false;
    }
    // random init
    // TODO Ugly Design. Only reasonable when height = 3
    for (int w : range(width)) {
        for (int h : range(height)) {
            if (h == ch)
                continue;
            if (h == 0 && w == 0)
                continue;
            if (h == height - 1 && w == width - 1)
                continue;
            if (map_info[h][w].is_null) {
                if (RandomHelper::random_real(0.0f, 1.0f) < 0.5) {
                    map_info[h][w].type = MAP_TYPE::NORMAL;
                    map_info[h][w].is_clear = false;
                    map_info[h][w].is_null = false;
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

void MapHelper::init_audio()
{
    auto ae = SimpleAudioEngine::getInstance();
    for (auto& audio_path : {
             FilePath::shot_effect,
             FilePath::button_effect,
             FilePath::enemy_die_effect,
             FilePath::enemy_hurt_effect,
             FilePath::reload_effect }) {
        ae->preloadEffect(audio_path);
    }
    for (auto& audio_path : {
             FilePath::background_fight }) {
        ae->preloadBackgroundMusic(audio_path);
    }
}
};