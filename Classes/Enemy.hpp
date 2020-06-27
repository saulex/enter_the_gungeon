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
    // move
    void run_ai_move_logic();
    //shot
    float shot_interval; // shot interval between shot rounds
    float shot_interval_timer;
    float shot_delay; // shot delay in one round
    bool one_shot_round_over;
    int shot_num;
    void fire_at_player();
    // game loop control
    void when_player_die();
    // hp
    void when_hurt(int damage) override;
    void when_die() override;
};

}

#endif