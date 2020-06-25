#include "Bullet.hpp"
#include "config.hpp"

using namespace cocos2d;

namespace etg {

Bullet::Bullet()
{
    this->tag_fire_by = -1;
    this->vol = { 0, 0 };
}

Bullet* Bullet::create(const std::string& filename)
{
    Bullet* sprite = new (std::nothrow) Bullet();
    if (sprite && sprite->initWithFile(filename) && sprite->Bullet::init()) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool Bullet::init()
{
    set_body();
    return true;
}

void Bullet::set_body()
{
    auto body = PhysicsBody::create();
    body->setGravityEnable(false);
    body->setDynamic(false);
    body->setRotationEnable(false);

    auto shape = PhysicsShapeEdgeBox::create(getContentSize());
    shape->setCategoryBitmask(int(C_MASK::bullet));
    shape->setContactTestBitmask(
        int(C_MASK::character) | int(C_MASK::wall) | int(C_MASK::bullet));
    body->addShape(shape);

    this->addComponent(body);
}
}
