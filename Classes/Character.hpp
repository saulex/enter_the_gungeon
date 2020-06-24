#ifndef _ETG_CHARACTER_HPP_
#define _ETG_CHARACTER_HPP_

#include "cocos2d.h"
#include <Status.hpp>
#include <config.hpp>
#include <map>
#include <my_utils.hpp>
#include <string>

using namespace cocos2d;
using namespace myutl;

namespace etg {

class Character : public Sprite {

public:
    CREATE_FUNC(Character);
    Character();
    static Character* create(const std::string& filename);

public:
    // MOVE
    enum TAG_ACT {
        U = 432,
        D = 521,
        L = 231,
        R = 987
    };
    std::map<DIR, int> contact_count_on;
    Vec2 move_speed;

    static TAG_ACT d_to_act_tag(DIR d);
    void move_for(DIR d, float t);
    void stop_move_for(DIR d);
    void contact_begin_on(DIR d);
    void contact_end_on(DIR d);

    // PHYSICS BODY
    virtual void set_physics_body();

    EventListenerPhysicsContact* contact_listener;
    void set_contact_listener();
};

class Player : public Character {

public:
    CREATE_FUNC(Player);
    Player();
    static Player* create(const std::string& filename);
    bool init() override;
    void update(float delta) override;

protected:

    Vec2 default_anchor = { 0.5, 0 };

    // keyboard listener
    std::map<EventKeyboard::KeyCode, bool> pressed;
    static DIR key2d(const EventKeyboard::KeyCode& key);
    static EventKeyboard::KeyCode d2key(const DIR& d);

    void add_mouse_listener();
    void add_key_listener();

    // physics body
    Vec2 body_offset_rate = { 0, -0.5 };
    Vec2 body_size_rate = { 1, 0.5 };

    void set_physics_body() override;
    
};

}

#endif //_ETG_CHARACTER_HPP_