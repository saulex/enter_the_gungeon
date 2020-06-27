#include <Enemy.hpp>
#include <random>

namespace etg {

Enemy::Enemy()
    : player(nullptr)
    , shot_interval(SHOT_INTERVAL_ENEMY)
    , shot_delay(SHOT_DELAY_ENEMY)
    , one_shot_round_over(true)
    , shot_num(SHOT_NUMBER_ENEMY)
{
    shot_interval_timer = RandomHelper::random_real(
        0.0f, 3 * SHOT_INTERVAL_ENEMY);
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
    run_ai_move_logic();
    scheduleUpdate();
    return true;
}

void Enemy::update(float delta)
{
    if (player && shot_interval_timer < 0 && one_shot_round_over) {
        auto sequence_shot_v = Vector<FiniteTimeAction*>();

        sequence_shot_v.pushBack(CallFunc::create(
            [&]() { one_shot_round_over = false; }));

        for (int i : range(shot_num)) {
            sequence_shot_v.pushBack(CallFunc::create(boost::bind(&Enemy::fire_at_player, this)));
            if (i != shot_num - 1)
                sequence_shot_v.pushBack(DelayTime::create(shot_delay));
        }

        sequence_shot_v.pushBack(CallFunc::create([&]() {
            one_shot_round_over = true;
            shot_interval_timer = shot_interval; // reset timer after one round
        }));

        this->runAction(Sequence::create(sequence_shot_v));
    }
    shot_interval_timer -= delta;
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

void Enemy::run_ai_move_logic()
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
        if (!player)
            return;
        int randtime = rand() % 2 + 1; //随机移动时间
        auto ep = getPosition(); // enemy position
        auto pp = player->getPosition(); // player position
        auto x_move_limit = 50.0f, y_move_limit = 50.0f;
        // mylog(v2s(ep - pp));
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

    auto move_act = Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create(ai_move),
        DelayTime::create(0.5f),
        CallFunc::create(random_stop),
        NULL);
    auto act = RepeatForever::create(move_act);
    this->runAction(act);
}

void Enemy::fire_at_player()
{
    if (!player)
        return;
    auto start = getPosition();
    auto offset = player->getPosition() - getPosition();
    auto vol = dot(offset / offset.length(), SPEED_BULLET_ENEMY);
    shot(start,
        vol,
        getTag(),
        DAMAGE_ENEMY_BULLET);
}

void Enemy::when_player_die()
{
    player = nullptr;
}

void Enemy::when_hurt(int damage)
{
    // hurt animation is not stoppable
    if (getActionByTag(int(TAG::enemy_hurt_anm)))
        return;

    auto get_hurt_act = Sequence::create(
        TintBy::create(0.25f,
            0, -255, -255),
        TintBy::create(0.25f,
            0, -255, -255)
            ->reverse(),
        NULL);

    get_hurt_act->setTag(int(TAG::enemy_hurt_anm));
    runAction(get_hurt_act);
}

void Enemy::when_die()
{
    player = nullptr;
    getEventDispatcher()->removeEventListenersForTarget(this);
    this->stopAllActions();
    this->removeAllComponents();
    log("die");
    auto sequence = Sequence::create(
        Spawn::createWithTwoActions(
            FadeOut::create(0.5f),
            ScaleBy::create(0.5f, 0.2f)),
        CallFunc::create([&]() { die(); }), // call parent to clean itself
        NULL);
    runAction(sequence);
}

}
