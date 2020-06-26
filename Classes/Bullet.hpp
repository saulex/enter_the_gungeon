#ifndef _ETG_BULLET_HPP_
#define _ETG_BULLET_HPP_

#include "cocos2d.h"
#include <config.hpp>

namespace etg {

class Bullet : public cocos2d::Sprite {

public:
    Bullet();
    CREATE_FUNC(Bullet);
    static Bullet* create(const std::string& filename);
    bool init() override;

public:
    void set_tag_fire_by(TAG tag);
    int get_tag_fire_by() { return this->tag_fire_by; };

    cocos2d::Vec2 vol;

protected:
    int tag_fire_by;
    void set_body();
};

}
#endif
