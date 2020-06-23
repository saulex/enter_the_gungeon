#ifndef _ETG_CHARACTER_HPP_
#define _ETG_CHARACTER_HPP_

#include "cocos2d.h"
#include <Status.hpp>
#include <map>
#include <string>
#include <config.hpp>

using namespace cocos2d;

namespace etg {

class Character : public Sprite {

public:
    CREATE_FUNC(Character);
    static Character* create(const std::string& filename);

    void move_by(const Vec2& delta);

public:
    enum class Direction {
        UP,
        DOWN,
        RIGHT,
        LEFT,
        UP_LEFT,
        UP_RIGHT,
        DOWN_LEFT,
        DOWN_RIGHT
    };

public:
    Direction direction;
};

class Player : public Character {

public:
    CREATE_FUNC(Player);
    Player();
    static Player* create(const std::string& filename);
    bool init() override;
    void update(float delta) override;

    std::map<DIR, bool> contact_face;

protected:
    std::map<EventKeyboard::KeyCode, bool> pressed;

    Vec2 move_speed = { 3, 3 };
    float body_damping = 0.1f;
    Vec2 body_offset_rate = { 0, -0.5 };
    Vec2 body_size_rate = { 1, 0.5 };
    Vec2 default_anchor = { 0.5, 0 };


    void set_body();

    void add_mouse_listener();
    void add_key_listener();
};

}

#endif //_ETG_CHARACTER_HPP_