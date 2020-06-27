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

#include <algorithm>

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
    player->setPosition(dot(map->getContentSize(), { 0.1, 0.1 }));
    player->setTag(int(TAG::player_node));
    player->setGlobalZOrder(map->pos_to_order(player->getPosition()));
    map->addChild(player);
    player->die.connect(boost::bind(&GungeonWorld::when_game_end, this));
    // Physics world
    getPhysicsWorld()->setGravity({ 0, 0 });
    // getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // SHOT, DAMAGE, HP
    set_bullet_listener();
    player->hp = HP_LIMIT_PLAYER;
    player->hp_limit = HP_LIMIT_PLAYER;
    player->shot.connect(boost::bind(&GungeonWorld::add_bullet, this,
        boost::placeholders::_1, boost::placeholders::_2,
        boost::placeholders::_3, boost::placeholders::_4));
    this->bullet_hit.connect(boost::bind(&GungeonWorld::on_bullet_hit, this,
        boost::placeholders::_1, boost::placeholders::_2));
    // Enemy
    generate_enemies();
    // set debugger
    // set_debugger();
    // make update() working
    scheduleUpdate();
    return true;
}

void GungeonWorld::update(float delta)
{
    // update z-order
    if (player) {
        player->setLocalZOrder(map->pos_to_order(player->getPosition()));
    }
    // clean bullets
    clean_bullets();
}

void GungeonWorld::set_debugger()
{
    auto to_run = [&]() {
        auto s = std::string("");
        s.append("[" + std::to_string(player->hp) + "]");
        for (auto& e : enemies) {
            s.append(" ," + std::to_string(e->hp));
        }
        mylog(s);
    };
    auto intv = 1.0f;
    this->runAction(RepeatForever::create(
        Sequence::createWithTwoActions(
            DelayTime::create(intv),
            CallFunc::create(to_run))));
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
        auto node_a = c.getShapeA()->getBody()->getNode();
        auto node_b = c.getShapeB()->getBody()->getNode();
        auto bullet_a = dynamic_cast<Bullet*>(node_a);
        auto bullet_b = dynamic_cast<Bullet*>(node_b);
        if (bullet_a && bullet_b) {
            return false;
        }
        if (bullet_a || bullet_b) {
            if (bullet_a) {
                bullet_hit(bullet_a, node_b);
            } else {
                bullet_hit(bullet_b, node_a);
            }
        }
        return false;
    };

    getEventDispatcher()->addEventListenerWithSceneGraphPriority(cl, this);
}

void GungeonWorld::add_bullet(
    const cocos2d::Vec2& start,
    const cocos2d::Vec2& vol,
    int tag_fire_by,
    int damage)
{
    if (tag_fire_by == int(TAG::player_node)) {
        auto bullet = Bullet::create(FilePath::player_bullet);
        bullet->vol = vol;

        bullet->setScale(0.3); // TODO Bad Design
        bullet->setAnchorPoint({ 0.5, 0.5 });
        bullet->setPosition(start);

        bullet->set_tag_fire_by(TAG::player_node);
        bullet->set_damage(damage);
        map->addChild(bullet);

        bullet->runAction(RepeatForever::create(
            MoveBy::create(1.0f, vol)));

        bullets.insert(bullet);
    } else if (tag_fire_by == int(TAG::enemy_node)) {
        auto bullet = Bullet::create(FilePath::enemy_bullet);
        bullet->vol = vol;

        bullet->setScale(0.3); // TODO Bad Design
        bullet->setAnchorPoint({ 0.5, 0.5 });
        bullet->setPosition(start);

        bullet->set_tag_fire_by(TAG::enemy_node);
        bullet->set_damage(damage);
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

void GungeonWorld::on_bullet_hit(Bullet* bullet, Node* hit_node)
{
    if (this->bullets_to_del.count(bullet))
        return;
    if (hit_node->getTag() != bullet->get_tag_fire_by()) {
        //log("bullet hit, from: %d, to: %d",
        //    bullet->get_tag_fire_by(), hit_node->getTag());
        bullet->setVisible(false);
        bullets_to_del.insert(bullet);
        // do damage
        auto hit_character = dynamic_cast<Character*>(hit_node);
        if (hit_character) {
            hit_character->do_damage(bullet->get_damage());
        }
    }
}

void GungeonWorld::generate_enemies()
{
    // 9 个可能出怪的位置,是地图的等分点
    auto& mn = ENEMY_GENERATE_POS;
    auto generate_points = std::vector<Vec2> {};
    for (int i : range(1, mn[0] + 1)) {
        for (int j : range(1, mn[1] + 1)) {
            generate_points.push_back(
                dot(map->getContentSize(),
                    { i * 1.0f / (mn[0] + 1), j * 1.0f / (mn[1] + 1) }));
        }
    }

    auto add_one_enemy_on = [&](const Vec2& point) {
        auto enemy = Enemy::create(AutoPolygon::generatePolygon(
            "Animation/enemy/slime/default.png"));
        enemy->setScale(0.5);
        enemy->setPosition(point);
        enemy->setTag(int(TAG::enemy_node));
        // set player
        enemy->set_player(player);
        player->die.connect(boost::bind(&Enemy::when_player_die, enemy));
        // enable shot
        enemy->shot_num += RandomHelper::random_int(0, 3);
        enemy->shot.connect(boost::bind(&GungeonWorld::add_bullet, this,
            boost::placeholders::_1, boost::placeholders::_2,
            boost::placeholders::_3, boost::placeholders::_4));
        // set hp
        enemy->hp = HP_LIMIT_ENEMY;
        enemy->hp_limit = HP_LIMIT_ENEMY;
        enemy->die.connect(boost::bind(&GungeonWorld::when_enemy_die, this, enemy));
        // maintain
        this->map->addChild(enemy);
        this->enemies.push_back(enemy);
    };

    // 保底出怪
    for (int i : range(ENEMY_GENERATE_LEAST)) {
        int j = RandomHelper::random_int(0, int(generate_points.size() - 1));
        auto point = generate_points[j];
        generate_points.erase(generate_points.begin() + j);

        add_one_enemy_on(point);
    }
    // 有几率出怪
    for (auto& point : generate_points) {
        if (RandomHelper::random_real(0.0f, 1.0f) < ENEMY_GENERATE_PROB) {
            add_one_enemy_on(point);
        }
    }
}

void GungeonWorld::when_game_end()
{
    clean_bullets();
    for (auto& e : enemies) {
        e->removeAllComponents();
        getEventDispatcher()->removeEventListenersForTarget(e);
        e->stopAllActions();
    }
    removeChild(player);
    player = nullptr;
}

void GungeonWorld::when_enemy_die(Enemy* e)
{
    enemies.erase(std::remove(enemies.begin(), enemies.end(), e), enemies.end());
    this->removeChild(e);

}
}
