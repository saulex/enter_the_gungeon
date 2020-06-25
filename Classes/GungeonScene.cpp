/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "GungeonScene.h"
#include <config.hpp>
#include <my_utils.hpp>

using namespace cocos2d;

namespace etg {

Scene* GungeonWorld::createScene()
{
    return GungeonWorld::create();
}

// on "init" you need to initialize your instance
bool GungeonWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::initWithPhysics()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // add camera
    this->camera_node = Camera::create();
    camera_node->setPosition({ 0, 0 });
    this->addChild(camera_node);
    // add map
    this->map = Map::create("maps/base_room/base_room.tmx");
    map->setAnchorPoint({ 0, 0 });
    map->setPosition({ 0, 0 });
    // map->setGlobalZOrder(int(Order::ground));
    camera_node->addChild(map);
    // scale camera by map width
    camera_node->scale_rate = visibleSize.width / map->getContentSize().width;
    camera_node->setScale(camera_node->scale_rate);
    // add player
    this->player = Player::create("Animation/player/Down/Character_Down1.png");
    player->setAnchorPoint({ 0.5, 0 });
    player->setPosition(0.25 * map->getContentSize());
    player->setGlobalZOrder(map->pos_to_order(player->getPosition()));
    map->addChild(player);
    // Physics world
    getPhysicsWorld()->setGravity({ 0, 0 });
    getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    getPhysicsWorld()->setUpdateRate(2);
    // Enemy
    auto enemy = Enemy::create("Animation/player/Down/Character_Down1.png");
    enemy->setPosition(0.5 * map->getContentSize());
    map->addChild(enemy);
    // make update() working
    scheduleUpdate();
    return true;
}

void GungeonWorld::update(float delta)
{
    player->setLocalZOrder(map->pos_to_order(player->getPosition()));
}

void GungeonWorld::set_contact_listener()
{
    // add contact listener
    contact_listener = EventListenerPhysicsContact::create();

    contact_listener->onContactBegin = [&](PhysicsContact& c) -> bool {
        auto a = c.getShapeA();
        auto b = c.getShapeB();
        for (auto& shape : { a, b }) {
            if (shape->getCategoryBitmask() & int(C_MASK::character)) {

            }
        }
        return true;
    };

    contact_listener->onContactSeparate = [&](PhysicsContact& c) -> bool {
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(contact_listener, this);
}
}
