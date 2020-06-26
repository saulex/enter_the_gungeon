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

void Bullet::set_tag_fire_by(TAG tag)
{
    if (tag == TAG::player_node) {
        this->tag_fire_by = int(TAG::player_node);
        this->getPhysicsBody()->getFirstShape()->setContactTestBitmask(
            int(C_MASK::all) ^ (int(C_MASK::player)));
    } else if (tag == TAG::enemy_anm) {
        this->tag_fire_by = int(TAG::enemy_anm);
        this->getPhysicsBody()->getFirstShape()->setContactTestBitmask(
            int(C_MASK::all) ^ (int(C_MASK::enemy)));
    }

}

void Bullet::set_body()
{
    auto body = PhysicsBody::create();
    body->setGravityEnable(false);
    body->setDynamic(false);
    body->setRotationEnable(false);

    auto shape = PhysicsShapeEdgeBox::create(getContentSize());
    shape->setCategoryBitmask(int(C_MASK::bullet));
    shape->setContactTestBitmask(int(C_MASK::all));
    body->addShape(shape);

    this->addComponent(body);
}
}
