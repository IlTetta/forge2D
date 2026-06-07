#pragma once

namespace forge {

class Entity; // defined in Entity.h — forward declare to break circular dependency

class Component {
public:
    virtual ~Component() = default;

    virtual void onUpdate(float /*dt*/) {}
    virtual void onRender() {}

    Entity* entity = nullptr; // set automatically by Entity::addComponent
};

} // namespace forge
