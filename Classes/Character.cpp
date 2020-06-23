#include <Character.hpp>
#include <my_utils.hpp>

namespace etg {

Character* Character::create(const std::string& filename)
{
    return static_cast<Character*>(Sprite::create(filename));
}

void Character::move_by(const Vec2& delta)
{
    setPosition(getPosition() + delta);
}

Player::Player()
{
    for (int i = 0; i < DIR_END; i++) {
        contact_face[DIR(i)] = false;
    }
}

Player* Player::create(const std::string& filename)
{
    Player* sprite = new (std::nothrow) Player();
    if (sprite && sprite->initWithFile(filename) && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool Player::init()
{
    set_body();
    add_mouse_listener();
    add_key_listener();

    scheduleUpdate();
    return true;
}

void Player::update(float delta)
{
    // update position
    auto move_dirct = Vec2(0, 0);
    std::vector<EventKeyboard::KeyCode> tmp_keys = {
        EventKeyboard::KeyCode::KEY_W,
        EventKeyboard::KeyCode::KEY_S,
        EventKeyboard::KeyCode::KEY_A,
        EventKeyboard::KeyCode::KEY_D
    };
    std::vector<DIR> tmp_dirs = {
        DIR::U, DIR::D, DIR::L, DIR::R
    };
    for (int i : myutl::range(tmp_keys.size())) {
        if (pressed[tmp_keys[i]] && !contact_face[tmp_dirs[i]]) {
            move_dirct += d2v(tmp_dirs[i]);
            contact_face[tmp_dirs[i]] = false;
        }
    }
    //if (pressed[EventKeyboard::KeyCode::KEY_W] && !contact_face[DIR::U]) {
    //    move_dirct += { 0, 1 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_S] && !contact_face[DIR::D]) {
    //    move_dirct += { 0, -1 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_A] && !contact_face[DIR::L]) {
    //    move_dirct += { -1, 0 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_D] && !contact_face[DIR::R]) {
    //    move_dirct += { 1, 0 };
    //}
    //log(myutl::v2s(move_dirct).c_str());
    this->runAction(
        MoveBy::create(delta, myutl::dot(move_dirct, move_speed)));
}

void Player::set_body()
{
    auto shape = PhysicsShapeBox::create(
        Size(myutl::dot(getContentSize(), body_size_rate)),
        { 1, 0, 1 },
        myutl::dot(getContentSize(), body_offset_rate));
    shape->setTag(TAG::player_body);
    shape->setContactTestBitmask(0xFFFFFFFF);

    auto body = PhysicsBody::create();
    body->addShape(shape);

    body->setLinearDamping(body_damping);
    body->setDynamic(true);
    this->addComponent(body);
}

void Player::add_mouse_listener()
{
}

void Player::add_key_listener()
{
    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        // log("%d pressed", keyCode);

        if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            pressed[EventKeyboard::KeyCode::KEY_W] = true;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            pressed[EventKeyboard::KeyCode::KEY_S] = true;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            pressed[EventKeyboard::KeyCode::KEY_A] = true;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            pressed[EventKeyboard::KeyCode::KEY_D] = true;
        }
    };

    listener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        // log("%d release", keyCode);

        if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            pressed[EventKeyboard::KeyCode::KEY_W] = false;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            pressed[EventKeyboard::KeyCode::KEY_S] = false;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            pressed[EventKeyboard::KeyCode::KEY_A] = false;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            pressed[EventKeyboard::KeyCode::KEY_D] = false;
        }
    };

    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

}
