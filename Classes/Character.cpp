#include <Character.hpp>
#include <my_utils.hpp>

using namespace myutl;

namespace etg {

Character::Character()
{
    for (auto& d : { DIR::L, DIR::R, DIR::U, DIR::D })
        contact_count_on[d] = 0;
    move_speed = DEFAULT_MOVE_SPEED;
}

Character* Character::create(const std::string& filename)
{
    return static_cast<Character*>(Sprite::create(filename));
}

Character::TAG_ACT Character::d_to_act_tag(DIR d)
{
    if (d == DIR::D)
        return TAG_ACT::D;
    if (d == DIR::U)
        return TAG_ACT::U;
    if (d == DIR::L)
        return TAG_ACT::L;
    if (d == DIR::R)
        return TAG_ACT::R;
}

void Character::move_for(DIR d, float t)
{
    if (contact_count_on[d] == 0) {
        auto move_delta = dot(d2v(d), move_speed);

        auto unit_acts = cocos2d::Vector<FiniteTimeAction*>();
        auto unit_move_len = 0.5f;
        auto n = int(move_delta.length() / unit_move_len);
        for (auto& i : range(n)) {
            auto a = MoveBy::create(t / n, d2v(d) * unit_move_len);
            a->setTag(d_to_act_tag(d));
            unit_acts.pushBack(a);
        }

        auto move_act = Sequence::create(unit_acts);
        move_act->setTag(d_to_act_tag(d));
        this->runAction(move_act);
    }
}

void Character::stop_move_for(DIR d)
{
    auto act_tag = d_to_act_tag(d);
    this->stopAllActionsByTag(act_tag);
}

void Character::contact_begin_on(DIR d)
{
    contact_count_on[d] += 1;
    stop_move_for(d);
}

void Character::contact_end_on(DIR d)
{
    contact_count_on[d] -= 1;
    if (contact_count_on[d] > 0)
        this->stopActionByTag(d);
}

void Character::set_physics_body()
{
    auto body = PhysicsBody::create();
    // like a sensor
    body->setRotationEnable(false);
    body->setDynamic(false);
    // set shape
    auto shape = PhysicsShapeEdgeBox::create(getContentSize());
    shape->setCategoryBitmask(int(C_MASK::character));
    shape->setContactTestBitmask(int(C_MASK::character) | int(C_MASK::wall));
    body->addShape(shape);
    // add body
    this->addComponent(body);
}

void Character::set_contact_listener()
{
    // add contact listener
    contact_listener = EventListenerPhysicsContact::create();

    contact_listener->onContactBegin = [&](PhysicsContact& c) -> bool {
        auto self = c.getShapeA();
        auto other = c.getShapeB();
        auto this_shape = this->getPhysicsBody()->getFirstShape();
        if (self == this_shape || other == this_shape) {
            log("contact on!");
            auto normal = c.getContactData()->normal; // self -> other
            if (other == this_shape) {
                auto tmp = other;
                other = self;
                self = tmp;
                normal = -normal;
            }
            // direction
            auto d = DIR::E;
            for (DIR i : { DIR::U, DIR::D, DIR::L, DIR::R }) {
                auto v = d2v(i);
                if (veq(v, normal))
                    d = i;
            }
            this->contact_begin_on(d);
        }
        // ignore
        return false;
    };

    contact_listener->onContactSeparate = [&](PhysicsContact& c) -> bool {
        auto self = c.getShapeA();
        auto other = c.getShapeB();
        auto this_shape = this->getPhysicsBody()->getFirstShape();
        if (self == this_shape || other == this_shape) {
            log("contact end!");
            auto normal = c.getContactData()->normal; // self -> other
            if (other == this_shape) {
                auto tmp = other;
                other = self;
                self = tmp;
                normal = -normal;
            }
            // direction
            auto d = DIR::E;
            for (DIR i : { DIR::U, DIR::D, DIR::L, DIR::R }) {
                auto v = d2v(i);
                if (veq(v, normal))
                    d = i;
            }
            this->contact_end_on(d);
        }
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(contact_listener, this);
}

Player::Player()
{
    pressed[EventKeyboard::KeyCode::KEY_W] = false;
    pressed[EventKeyboard::KeyCode::KEY_S] = false;
    pressed[EventKeyboard::KeyCode::KEY_A] = false;
    pressed[EventKeyboard::KeyCode::KEY_D] = false;
}

Player* Player::create(const std::string& filename)
{
    Player* self = new (std::nothrow) Player();
    if (self && self->initWithFile(filename) && self->init()) {
        self->autorelease();
        return self;
    }
    CC_SAFE_DELETE(self);
    return nullptr;
}

bool Player::init()
{
    set_physics_body();
    set_contact_listener();

    add_mouse_listener();
    add_key_listener();

    scheduleUpdate();
    return true;
}

void Player::update(float delta)
{
    for (auto& d : { DIR::U, DIR::L, DIR::R, DIR::D }) {
        auto key = d2key(d);
        if (pressed[key]) {
            move_for(d, delta);
        }
    }

    // update position
    // auto move_dirct = Vec2(0, 0);

    //std::vector<EventKeyboard::KeyCode> tmp_keys = {
    //    EventKeyboard::KeyCode::KEY_W,
    //    EventKeyboard::KeyCode::KEY_S,
    //    EventKeyboard::KeyCode::KEY_A,
    //    EventKeyboard::KeyCode::KEY_D
    //};
    //std::vector<DIR> tmp_dirs = {
    //    DIR::U, DIR::D, DIR::L, DIR::R
    //};
    //for (int i : myutl::range(tmp_keys.size())) {
    //    if (pressed[tmp_keys[i]] && !contact_face[tmp_dirs[i]]) {
    //        move_dirct += d2v(tmp_dirs[i]);
    //        contact_face[tmp_dirs[i]] = false;
    //    }
    //}

    //if (pressed[EventKeyboard::KeyCode::KEY_W]) {
    //    move_dirct += { 0, 1 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_S]) {
    //    move_dirct += { 0, -1 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_A]) {
    //    move_dirct += { -1, 0 };
    //}
    //if (pressed[EventKeyboard::KeyCode::KEY_D]) {
    //    move_dirct += { 1, 0 };
    //}
    //log(myutl::v2s(move_dirct).c_str());
    //auto move_delta = myutl::dot(move_dirct, move_speed);
    //for (Vec2 d : { Vec2(1, 0), Vec2(0, 1) }) {
    //    this->move_by(myutl::dot(move_delta, d));
    //}
    //this->runAction(
    //    MoveBy::create(delta, myutl::dot(move_dirct, move_speed)));
}

//void Player::set_body()
//{
//    getBoundingBox();
//    auto shape = PhysicsShapeBox::create(
//        Size(myutl::dot(getContentSize(), body_size_rate)),
//        { 1, 0, 1 },
//        myutl::dot(getContentSize(), body_offset_rate));
//    shape->setTag(TAG::player_body);
//    shape->setContactTestBitmask(0xFFFFFFFF);
//
//    auto body = PhysicsBody::create();
//    body->addShape(shape);
//
//    body->setLinearDamping(body_damping);
//    body->setDynamic(true);
//    this->addComponent(body);
//}

DIR Player::key2d(const EventKeyboard::KeyCode& key)
{
    if (key == EventKeyboard::KeyCode::KEY_W)
        return DIR::U;
    if (key == EventKeyboard::KeyCode::KEY_S)
        return DIR::D;
    if (key == EventKeyboard::KeyCode::KEY_A)
        return DIR::L;
    if (key == EventKeyboard::KeyCode::KEY_D)
        return DIR::R;
}

EventKeyboard::KeyCode Player::d2key(const DIR& d)
{
    if (d == DIR::U)
        return EventKeyboard::KeyCode::KEY_W;
    if (d == DIR::D)
        return EventKeyboard::KeyCode::KEY_S;
    if (d == DIR::L)
        return EventKeyboard::KeyCode::KEY_A;
    if (d == DIR::R)
        return EventKeyboard::KeyCode::KEY_D;
}

void Player::add_mouse_listener()
{
}

void Player::add_key_listener()
{
    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        // log("%d pressed", keyCode);

        if (keyCode == EventKeyboard::KeyCode::KEY_W)
            pressed[EventKeyboard::KeyCode::KEY_W] = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_S)
            pressed[EventKeyboard::KeyCode::KEY_S] = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            pressed[EventKeyboard::KeyCode::KEY_A] = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            pressed[EventKeyboard::KeyCode::KEY_D] = true;
    };

    listener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        // log("%d release", keyCode);

        if (keyCode == EventKeyboard::KeyCode::KEY_W)
            pressed[EventKeyboard::KeyCode::KEY_W] = false;
        if (keyCode == EventKeyboard::KeyCode::KEY_S)
            pressed[EventKeyboard::KeyCode::KEY_S] = false;
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            pressed[EventKeyboard::KeyCode::KEY_A] = false;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            pressed[EventKeyboard::KeyCode::KEY_D] = false;
    };

    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Player::set_physics_body()
{
    auto body = PhysicsBody::create();
    // like a sensor
    body->setRotationEnable(false);
    body->setDynamic(false);
    // set shape
    auto shape = PhysicsShapeEdgeBox::create(
        Size(dot(getContentSize(), body_size_rate)),
        PHYSICSSHAPE_MATERIAL_DEFAULT,
        0,
        dot(body_offset_rate, getContentSize()));
    shape->setCategoryBitmask(int(C_MASK::character));
    shape->setContactTestBitmask(int(C_MASK::character) | int(C_MASK::wall));
    body->addShape(shape);
    // add body
    this->addComponent(body);
}
}
