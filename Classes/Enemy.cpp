#include <Enemy.hpp>

namespace etg {

Enemy::Enemy()
{
}

// ���Enemy::create�㲻��Ҫ��
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
    // ==== ���治��Ҫ�Ķ� ====
    Character::set_physics_body();
    Character::set_contact_listener();
    // ==== ���治��Ҫ�Ķ� ====

    // ==== ������Ҫ����д

    // ����˵��������õ��Ķ���
    move_speed = { 2, 2 }; // �����ƶ��ٶ�

    // ==== ���治��Ҫ�Ķ� ====
    scheduleUpdate();
    return true;
    // ==== ���治��Ҫ�Ķ� ====
}

void Enemy::update(float delta)
{
    // ==== ������Ҫ����д

    // ����˵��������õ��Ķ���
    
    // �����ƶ�delta�룬����ƶ��������Է����ã���ײǽ�˻��Զ�ֹͣ
    move_for(DIR::R, delta);
    // ֹͣ���ұ��ƶ�
    stop_move_for(DIR::U);
}
}
