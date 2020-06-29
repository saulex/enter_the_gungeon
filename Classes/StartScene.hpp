#ifndef _ETG_START_SCENE_HPP_
#define _ETG_START_SCENE_HPP_

#include "cocos2d.h"

namespace etg {

class StartScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void update(float delta) override;

    CREATE_FUNC(StartScene)

protected:
    cocos2d::Sprite* play_button = nullptr;
};

};

#endif //_ETG_START_SCENE_HPP_