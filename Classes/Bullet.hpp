#ifndef _ETG_BULLET_HPP_
#define _ETG_BULLET_HPP_

#include "cocos2d.h"

namespace etg {

class Bullet : public cocos2d::Sprite {

public:
    Bullet();
    CREATE_FUNC(Bullet);
    static Bullet* create(const std::string& filename);
    bool init() override;

public:
    int tag_fire_by;

protected:
    void set_body();
};

}
#endif
