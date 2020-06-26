#include <Enemy.hpp>
#include <random>

namespace etg {

Enemy::Enemy()
    : player(nullptr)
{
}

Enemy* Enemy::create(const std::string& filename)
{
    Enemy* self = new (std::nothrow) Enemy();
    if (self && self->initWithFile(filename) && self->init()) {
        self->autorelease();
        return self;
    }
    CC_SAFE_DELETE(self);
    return nullptr;
}

Enemy* Enemy::create(const PolygonInfo& info)
{
    Enemy* self = new (std::nothrow) Enemy();
    if (self && self->initWithPolygon(info) && self->init()) {
        self->autorelease();
        return self;
    }
    CC_SAFE_DELETE(self);
    return nullptr;
}

bool Enemy::init()
{
    // TODO Bad Design: Character should init physics body by itself.
    set_physics_body();
    set_contact_listener();

    move_speed = SPEED_MOVE_ENEMY; // 设置移动速度
    run_ai_logic();
    return true;
}

void Enemy::update(float delta)
{
}

void Enemy::set_physics_body()
{
    auto body = PhysicsBody::create();
    // like a sensor
    body->setRotationEnable(false);
    body->setDynamic(false);
    this->updatePoly();
    // set shape
    auto shape = PhysicsShapeEdgeBox::create(getContentSize());
    shape->setCategoryBitmask(int(C_MASK::enemy));
    shape->setContactTestBitmask(int(C_MASK::all) ^ int(C_MASK::enemy)); // 异或就是做减（加）法
    body->addShape(shape);
    // add body
    this->addComponent(body);
}

void Enemy::set_player(Player* pl)
{
    this->player = pl;
}

void Enemy::run_ai_logic()
{
    auto random_stop = [&]() {
        int randnum = rand() % 4;
        if (randnum == 0) {
            stop_move_for(DIR::U);
        }
        if (randnum == 1) {
            stop_move_for(DIR::D);
        }
        if (randnum == 2) {
            stop_move_for(DIR::L);
        }
        if (randnum == 3) {
            stop_move_for(DIR::R);
        }
    };

    auto ai_move = [&]() {
        int randtime = rand() % 2 + 1; //随机移动时间
        auto ep = getPosition(); // enemy position
        auto pp = player->getPosition(); // player position
        auto x_move_limit = 50.0f, y_move_limit = 50.0f;
        mylog(v2s(ep - pp));
        if (abs(ep.x - pp.x) > x_move_limit) {
            if (pp.x < ep.x)
                move_for(DIR::L, randtime);
            else
                move_for(DIR::R, randtime);
        }
        if (abs(ep.y - pp.y) > y_move_limit) {
            if (pp.y < ep.y)
                move_for(DIR::D, randtime);
            else
                move_for(DIR::U, randtime);
        }
    };

    int randtime = rand() % 2 + 1;
    this->scheduleUpdate();
    auto sss = Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create(ai_move),
        DelayTime::create(0.5f),
        CallFunc::create(random_stop),
        NULL);
    auto act = RepeatForever::create(sss);
    this->runAction(act);
}
}
