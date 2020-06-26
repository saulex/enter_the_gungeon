#ifndef _ETG_ENEMY_HPP_
#define _ETG_ENEMY_HPP_

#include <Character.hpp>
#include <Status.hpp>
#include <config.hpp>
#include <map>
#include <my_utils.hpp>
#include <string>

#include "cocos2d.h"

using namespace cocos2d;

namespace etg {

class Enemy : public Character {
public:
    CREATE_FUNC(Enemy);
    Enemy();
    static Enemy* create(const std::string& filename);
    static Enemy* create(const PolygonInfo& info);

public:
    bool init() override;
    void update(float delta) override;
    // physics body
    void set_physics_body() override;
    // ai
    Player* player;
    void set_player(Player* pl);

    void run_ai_logic();
};

}

#endif