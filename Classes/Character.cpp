#include <Character.hpp>
#include <config.hpp>
#include <math.h>
#include <my_utils.hpp>

using namespace myutl;

namespace etg {

Character::Character()
    : move_speed(DEFAULT_MOVE_SPEED)
    , hp(0)
    , hp_limit(0)
{
    for (auto& d : { DIR::L, DIR::R, DIR::U, DIR::D })
        contact_count_on[d] = 0;
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
    shape->setContactTestBitmask(
        int(C_MASK::character) | int(C_MASK::wall) | int(C_MASK::bullet));
    body->addShape(shape);
    // add body
    this->addComponent(body);
}

void Character::set_contact_listener()
{
    const bool DO_LOG = false;
    // add contact listener
    contact_listener = EventListenerPhysicsContact::create();

    contact_listener->onContactBegin = [&](PhysicsContact& c) -> bool {
        auto self = c.getShapeA();
        auto other = c.getShapeB();
        auto this_shape = this->getPhysicsBody()->getFirstShape();
        if (self == this_shape || other == this_shape) {
            // mylog("contact on!", DO_LOG);
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
            // mylog("contact end!", DO_LOG);
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

void Character::do_damage(int damage)
{
    hp -= damage;
    if (hp < 0)
        hp = 0;
    else if (hp > hp_limit)
        hp = hp_limit;
}

// ============ Player ============

Player::Player()
    : cur_move_anm(nullptr)
    , default_sf(nullptr)
    , cur_face_on(DIR::D)
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
    init_anm();
    set_physics_body();
    set_contact_listener();

    add_mouse_listener();
    add_move_listener();
    // shot
    add_shot_listener();

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
    // animate
    const bool DO_ANM_LOG = true;
    auto any_key_pressed = false;
    for (DIR d : { DIR::D, DIR::U, DIR::L, DIR::R })
        if (pressed[d2key(d)])
            any_key_pressed = true;
    if (any_key_pressed) {
        play_move_anm(cur_face_on);
    } else {
        if (cur_move_anm) {
            this->stopAllActionsByTag(int(TAG::player_anm));
            cur_move_anm = nullptr;
        }
        this->setSpriteFrame(d2sfs[cur_face_on].at(0));
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
    auto mouse_listener = EventListenerMouse::create();

    mouse_listener->onMouseMove = [&](EventMouse* e) {
        // vector
        auto local_pos = convertToNodeSpace(Vec2(e->getCursorX(), e->getCursorY()));
        auto offset = local_pos - 0.5 * getContentSize();
        // math
        float u = M_PI / 8; // unit radians
        auto rad = atan2f(offset.y, offset.x); // (-pi, pi]
        if (rad < -u)
            rad += M_PI * 2; // now [(-1/8) * pi, (15/8)pi )
        // dir
        std::vector<DIR> dirs = {
            DIR::R, DIR::UR, DIR::U, DIR::UL,
            DIR::L, DIR::DL, DIR::D, DIR::DR
        };
        auto face_dir = DIR::E;
        for (int i : range(dirs.size())) {
            if (rad < u + 2 * i * u) {
                face_dir = dirs[i];
                break;
            }
        }
        this->cur_face_on = face_dir;
    };

    this->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(mouse_listener, this);
}

void Player::add_move_listener()
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
    shape->setCategoryBitmask(int(C_MASK::player));
    shape->setContactTestBitmask(int(C_MASK::all));
    body->addShape(shape);
    // add body
    this->addComponent(body);
}

void Player::init_anm()
{
    // animation
    std::vector<DIR> all_dirs = {
        DIR::U, DIR::D, DIR::L, DIR::R,
        DIR::UL, DIR::UR, DIR::DL, DIR::DR
    };
    std::vector<std::string> filenames = {
        "Up", "Down", "Left", "Right",
        "UpLeft", "UpRight", "DownLeft", "DownRight"
    };
    for (int i : range(all_dirs.size())) {
        auto anim_info = animation_generator(
            "Animation/player/" + filenames[i] + "/",
            filenames[i],
            0.15,
            4,
            "Character_");
        auto anim = anim_info.animation;
        auto sfs = anim_info.sfs;
        anim->retain();

        this->d2anm[all_dirs[i]] = anim;
        this->d2sfs[all_dirs[i]] = sfs;
    }

    // this->default_sf = SpriteFrameCache::getInstance()
    //                     ->getSpriteFrameByName("Character_Down1.png");
}

void Player::play_move_anm(DIR d)
{
    const bool DO_ANM_LOG = false;
    if (cur_move_anm != d2anm[d]) {
        mylog("cur_move_anm != d2anm[cur_face_on]", DO_ANM_LOG);
        if (cur_move_anm) {
            mylog("[STOP] cur_move_anm != nullptr", DO_ANM_LOG);
            this->stopAllActionsByTag(int(TAG::player_anm));
        }
        mylog("[setSpriteFrame]", DO_ANM_LOG);
        this->setSpriteFrame(d2sfs[d].at(0));
        mylog("[RepeatForever]", DO_ANM_LOG);
        auto act = RepeatForever::create(Animate::create(d2anm[d]));
        act->setTag(int(TAG::player_anm));
        this->runAction(act);

        cur_move_anm = d2anm[d];
    }
}

void Player::add_shot_listener()
{
    auto ml = EventListenerMouse::create();

    ml->onMouseDown = [&](EventMouse* e) {
        if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            auto local_pos = convertToNodeSpace(Vec2(e->getCursorX(), e->getCursorY()));
            // TODO Correctly shot from CENTER
            auto offset = local_pos - 0.5 * getContentSize();
            shot(
                this->getPosition() + dot(getContentSize(), { 1.0 / 16, 0.5 }),
                dot(offset / offset.length(), SPEED_BULLET_PLAYER),
                int(TAG::player_node),
                DAMAGE_PLAYER_BULLET);
        }
    };

    getEventDispatcher()->addEventListenerWithSceneGraphPriority(ml, this);
}
}
