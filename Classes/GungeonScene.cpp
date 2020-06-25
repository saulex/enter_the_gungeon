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

#include <boost/Signals2.hpp>

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
    // add Camera
    this->camera = Camera::create();
    camera->setTag(int(TAG::camera_node));
    camera->setPosition({ 0, 0 });
    this->addChild(camera);
    // add Map
    this->map = Map::create("maps/base_room/base_room.tmx");
    map->setAnchorPoint({ 0, 0 });
    map->setPosition({ 0, 0 });
    map->setTag(int(TAG::camera_node));
    // map->setGlobalZOrder(int(Order::ground));
    camera->addChild(map);
    // scale camera by map width
    camera->scale_rate = visibleSize.width / map->getContentSize().width;
    camera->setScale(camera->scale_rate);
    // add player
    this->player = Player::create("Animation/player/Down/Character_Down1.png");
    player->setAnchorPoint(player->default_anchor);
    player->setPosition(0.25 * map->getContentSize());
    player->setTag(int(TAG::player_node));
    player->setGlobalZOrder(map->pos_to_order(player->getPosition()));
    map->addChild(player);
    // Physics world
    getPhysicsWorld()->setGravity({ 0, 0 });
    getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // shot
    set_bullet_listener();
    player->shot.connect(boost::bind(&GungeonWorld::add_bullet, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
    // make update() working
    scheduleUpdate();
    return true;
}

void GungeonWorld::update(float delta)
{
    // update z-order
    player->setLocalZOrder(map->pos_to_order(player->getPosition()));
    // clean bullets
    clean_bullets();
}

void GungeonWorld::set_bullet_listener()
{
    auto remove_bullet = [&](Bullet* b) {
        bullets.erase(b);
        b->removeAllComponents();
        b->removeAllChildren();
        b->removeFromParentAndCleanup(true);
    };
    const bool DO_LOG = false;
    auto cl = EventListenerPhysicsContact::create();
    cl->onContactBegin = [&](PhysicsContact& c) -> bool {
        auto a = c.getShapeA()->getBody()->getNode();
        auto b = c.getShapeB()->getBody()->getNode();
        auto bullet_a = dynamic_cast<Bullet*>(a);
        auto bullet_b = dynamic_cast<Bullet*>(b);
        if (bullet_a && bullet_b) {
            return false;
        }
        if (bullet_a || bullet_b) {
            Bullet* bullet;
            Node* hit_target;
            if (bullet_a) {
                bullet = bullet_a;
                hit_target = b;
            } else {
                bullet = bullet_b;
                hit_target = a;
            }
            if (hit_target->getTag() != bullet->tag_fire_by) {
                bullet->setVisible(false);
                bullets_to_del.insert(bullet);
                return false;
            }
        }
        return false;
    };

    getEventDispatcher()->addEventListenerWithSceneGraphPriority(cl, this);
}

void GungeonWorld::add_bullet(
    const cocos2d::Vec2& start,
    const cocos2d::Vec2& vol,
    int tag_fire_by)
{
    if (tag_fire_by == int(TAG::player_node)) {
        auto bullet = Bullet::create(FilePath::player_bullet);
        bullet->vol = vol;

        bullet->setScale(0.3); // TODO Bad Design
        bullet->setAnchorPoint({ 0.5, 0.5 });
        bullet->setPosition(start);

        bullet->tag_fire_by = int(TAG::player_node);
        map->addChild(bullet);

        bullet->runAction(RepeatForever::create(
            MoveBy::create(1.0f, vol)));

        bullets.insert(bullet);
    }
}

void GungeonWorld::clean_bullets()
{
    while (!bullets_to_del.empty()) {
        auto b = *bullets_to_del.begin();
        bullets_to_del.erase(b);
        bullets.erase(b);
        b->removeAllComponents();
        b->removeAllChildren();
        b->removeFromParent();
    }
}
}
