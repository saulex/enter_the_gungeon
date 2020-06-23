#include <Camera.hpp>
#include <my_utils.hpp>

namespace etg {

bool Camera::init()
{
    if (!Node::init()) {
        return false;
    }
    this->add_mouse_listener();
    return true;
}

void Camera::move_by(const Vec2& delta)
{
    // 要让摄像头移动，相对地，要让其子物体向相反方向运动
    myutl::move_by(this, -delta);
}

void Camera::add_mouse_listener()
{
    mouse_listen_has_begin = false;

    auto listener = EventListenerMouse::create();

    listener->onMouseMove = [&](Event* event) {
        /*
         * in mouse event, they use OpenGl coordinate
         * O ---> x
         * |
         * |
         * v y
         */
        auto e = dynamic_cast<EventMouse*>(event);
        auto cur_pos = e->getLocation();

        if (!mouse_listen_has_begin) {
            mouse_listen_has_begin = true;
            last_mouse_pos = cur_pos;
            return;
        }
        auto relative_move = cur_pos - last_mouse_pos;
        relative_move *= this->move_rate;
        last_mouse_pos = cur_pos;
        this->move_by(myutl::dot(relative_move, { 1, -1 }));
    };
    this->_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
}
}
