#include <Enemy.hpp>

namespace etg {

Enemy::Enemy()
{
}

// 这个Enemy::create你不需要动
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

bool Enemy::init()
{
    // ==== 下面不需要改动 ====
    Character::set_physics_body();
    Character::set_contact_listener();
    // ==== 上面不需要改动 ====

    // ==== 下面需要你来写

    // 举例说明你可能用到的东西
    move_speed = { 2, 2 }; // 设置移动速度

    // ==== 下面不需要改动 ====
    scheduleUpdate();
    return true;
    // ==== 上面不需要改动 ====
}

void Enemy::update(float delta)
{
    // ==== 下面需要你来写

    // 举例说明你可能用到的东西
    
    // 向右移动delta秒，这个移动函数可以放心用，它撞墙了会自动停止
    move_for(DIR::R, delta);
    // 停止向右边移动
    stop_move_for(DIR::U);
}
}
