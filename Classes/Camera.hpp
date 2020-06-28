#ifndef _ETG_CAMERA_HPP_
#define _ETG_CAMERA_HPP_

#include "cocos2d.h"
#include <iostream>

using namespace cocos2d;

namespace etg {

class Camera : public Node {

public:
    float scale_rate = 1.0f;
    float move_rate = 0.08f;

    virtual bool init();
    CREATE_FUNC(Camera);
    void move_by(const Vec2& delta);
    
    void add_mouse_listener();

protected:
    Vec2 last_mouse_pos;
    bool mouse_listen_has_begin;
};

};

#endif // _ETG_CAMERA_HPP_