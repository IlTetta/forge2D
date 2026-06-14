#pragma once

namespace forge {

class Entity; // defined in Entity.h — forward declare to break circular dependency

/**
 * @brief Abstract base class for all entity components.
 *
 * Derive from Component to implement custom logic that can be attached to
 * an Entity. Override onUpdate() for per-frame logic and onRender() to issue
 * draw calls. The @ref entity back-pointer is set automatically by
 * Entity::addComponent().
 *
 * @see Entity::addComponent(), Entity::getComponent()
 */
class Component {
public:
    virtual ~Component() = default;

    /**
     * @brief Per-frame update callback.
     * @param dt Delta time in seconds since the last update.
     */
    virtual void onUpdate(float /*dt*/) {}

    /** @brief Per-frame render callback. Issue draw calls here. */
    virtual void onRender() {}

    Entity* entity = nullptr; ///< Owning entity; set automatically by Entity::addComponent().
};

} // namespace forge
