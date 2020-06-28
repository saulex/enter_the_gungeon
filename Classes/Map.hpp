#ifndef _ETG_MAP_HPP_
#define _ETG_MAP_HPP_

#include "cocos2d.h"
#include <config.hpp>
#include <map>
#include <my_utils.hpp>

using namespace cocos2d;

namespace etg {

class Map : public TMXTiledMap {
public:
    static Map* create(const std::string& tmxFile);
    bool init() override;
    void update(float delta) override;

    void set_door(myutl::DIR dir, bool visible);

    /**
     * \brief
     * \param pos: Map Node coordinate
     * \return z order
     */
    float pos_to_order(const Vec2& pos);

protected:
    std::map<std::string, TMXLayer*> layer;
    PhysicsBody* wall_body;

    void update_body();
    void refresh_zorder();
};

};

#endif _ETG_MAP_HPP_