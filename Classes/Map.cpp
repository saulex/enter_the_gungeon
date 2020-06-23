#include <Map.hpp>
#include <my_utils.hpp>

namespace etg {

Map* Map::create(const std::string& tmxFile)
{
    Map* ret = new (std::nothrow) Map();
    if (ret->initWithTMXFile(tmxFile) && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Map::init()
{
    // set layer order
    //auto set_order = [&](const std::vector<std::string>& names, Order order) {
    //    for (const auto& name : names) {
    //        getLayer(name)->setGlobalZOrder(int(order));
    //    }
    //};

    //set_order({ "ground" }, Order::ground);
    //set_order({ "left_ground", "right_ground", "up_ground", "down_ground" }, Order::ground_1);
    //set_order({ "base_wall" }, Order::base_wall);
    //set_order({ "left_wall", "right_wall" }, Order::base_wall_1);
    //set_order({ "other_wall" }, Order::other_wall);
    //set_order({ "right_other", "left_other", "up_other", "down_other" }, Order::other_wall_1);
    //set_order({ "decoration" }, Order::decoration);

    // open wall
    for (auto door : { Door::D, Door::U, Door::L }) {
        set_door(door, false);
    }
    refresh_zorder();

    log(("map size: " + myutl::v2s(getMapSize())).c_str());
    log(("map tile size: " + myutl::v2s(getTileSize())).c_str());
    log(("map content size: " + myutl::v2s(getContentSize())).c_str());
    auto tmp_tile = getLayer("other_wall")->getTileAt({ 0, 0 });
    log(("tmp_tile, anchor: " + myutl::v2s(tmp_tile->getAnchorPoint())).c_str());
    log(("tmp_tile, position: " + myutl::v2s(tmp_tile->getPosition())).c_str());
    log(("tmp_tile, content size: " + myutl::v2s(tmp_tile->getContentSize())).data());
    // physics body
    update_body();

    scheduleUpdate();
    return true;
}

void Map::update(float delta)
{
}

void Map::set_door(Door door, bool visible)
{
    std::vector<std::string> layer_names;
    if (door == Door::U) {
        layer_names = { "up_other", "up_ground" };
    }
    if (door == Door::D) {
        layer_names = { "down_other", "down_ground" };
    }
    if (door == Door::L) {
        layer_names = { "left_other", "left_wall", "left_ground" };
    }
    if (door == Door::R) {
        layer_names = { "right_other", "right_wall", "right_ground" };
    }
    for (auto& s : layer_names) {
        getLayer(s)->setVisible(visible);
    }
}

float Map::pos_to_order(const Vec2& pos)
{
    return getContentSize().height - pos.y;
}

void Map::update_body()
{
    auto body = PhysicsBody::create();
    auto map_center = getContentSize() / 2.0;

    for (auto& child : _children) {
        TMXLayer* layer = dynamic_cast<TMXLayer*>(child);
        if (!layer || !layer->isVisible()) {
            continue;
        }
        for (int x : myutl::range(getMapSize().width)) {
            for (int y : myutl::range(getMapSize().height)) {
                /*
                 * 0 0 0
                 * 1 1 1
                 * 2 2 2
                 */
                auto tile = layer->getTileAt(Vec2(x, y));
                auto tile_gid = layer->getTileGIDAt(Vec2(x, y));
                auto is_walkable = (base_room_walkable_gid.count(tile_gid) == 1);
                if (!tile || is_walkable) {
                    continue;
                }
                auto tile_center_pos = tile->getPosition() + 0.5 * tile->getContentSize();
                auto shape = PhysicsShapeEdgeBox::create(
                    tile->getContentSize(),
                    PhysicsMaterial(10000, 0, 10000),
                    0,
                    tile_center_pos - map_center);
                shape->setContactTestBitmask(0xFFFFFFFF);
                shape->setTag(TAG::wall_body);
                body->addShape(shape, false);
                //body->addShape(PhysicsShapeBox::create(tile->getBoundingBox()));
            }
        }
    }

    if (wall_body) {
        this->removeComponent(wall_body);
    }
    body->setDynamic(false);
    wall_body = body;
    this->addComponent(wall_body);
}

void Map::refresh_zorder()
{
    /*
     * In tiled map
     * O ---> x
     * |
     * |
     * v y
     */
    for (auto& child : _children) {
        TMXLayer* layer = dynamic_cast<TMXLayer*>(child);
        if (!layer) {
            continue;
        }
        for (int x : myutl::range(getMapSize().width)) {
            for (int y : myutl::range(getMapSize().height)) {
                /*
                 * 0 0 0
                 * 1 1 1
                 * 2 2 2
                 */
                auto tile = layer->getTileAt(Vec2(x, y));
                if (!tile) {
                    continue;
                }
                auto order_anchor = tile->getPosition() + myutl::dot({ 0.5, 1 }, tile->getContentSize());
                tile->setLocalZOrder(pos_to_order(order_anchor));
            }
        }
    }
}
}
