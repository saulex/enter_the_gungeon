#include "GungeonScene.h"
#include <StartScene.hpp>
#include <config.hpp>
#include <my_utils.hpp>

#include "ui/CocosGUI.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

using namespace cocos2d;
using namespace myutl;

namespace etg {
cocos2d::Scene* StartScene::createScene()
{
    return StartScene::create();
}

bool StartScene::init()
{
    if (!Scene::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto bg = Sprite::create(FilePath::start_scene_background);
    bg->setScale(1.2);
    bg->setAnchorPoint({ 0.5, 0.5 });
    bg->setPosition(visibleSize / 2.0f);
    addChild(bg);

    auto title = Sprite::create(FilePath::start_scene_title);
    title->setAnchorPoint({ 0.5, 0.5 });
    title->setScale(0.95f);
    title->setPosition(dot(visibleSize, { 0.5, 0.8 }));
    addChild(title);

    play_button = Sprite::create(FilePath::play_button_normal);
    play_button->setAnchorPoint({ 0.5, 0.5 });
    play_button->setPosition(visibleSize / 2.0f);
    play_button->setScale(0.9);
    this->addChild(play_button);

    SimpleAudioEngine::getInstance()->preloadEffect(FilePath::button_effect);
    // SimpleAudioEngine::getInstance()->preloadEffect(FilePath::background_start);
    SimpleAudioEngine::getInstance()->playBackgroundMusic(FilePath::background_start, true);

    auto ml = EventListenerMouse::create();
    ml->onMouseMove = [&](EventMouse* e) {
        auto vs = Director::getInstance()->getVisibleSize();
        auto pos = Vec2(e->getLocation().x, vs.height - e->getLocation().y);

        if (play_button->getBoundingBox().containsPoint(pos)) {
            play_button->setTexture(FilePath::play_button_click);
            SimpleAudioEngine::getInstance()->playEffect(FilePath::button_effect);
        } else {
            play_button->setTexture(FilePath::play_button_normal);
        }
    };
    ml->onMouseUp = [&](EventMouse* e) {
        auto vs = Director::getInstance()->getVisibleSize();
        auto pos = Vec2(e->getLocation().x, vs.height - e->getLocation().y);

        if (play_button->getBoundingBox().containsPoint(pos)) {
            // SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            Director::getInstance()->replaceScene(GungeonWorld::createScene());
        }
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(ml, play_button);

    scheduleUpdate();
    return true;
}

void StartScene::update(float delta)
{
}
};