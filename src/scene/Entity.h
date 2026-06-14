#pragma once
#include "Component.h"
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace forge {

/**
 * @brief A named game object that owns a collection of Components.
 *
 * Every Entity is automatically given a Transform on creation by the Scene.
 * Components are stored as unique_ptrs and accessed via typed template methods.
 * Setting @ref active to false prevents onUpdate() and onRender() from being
 * called on the entity's components.
 *
 * @code
 * auto& player = scene.createEntity("Player");
 * auto& rb = player.addComponent<forge::Rigidbody2D>();
 * rb.velocity = {100.f, 0.f};
 *
 * if (auto* t = player.getComponent<forge::Transform>())
 *     t->position = {0.f, 100.f};
 * @endcode
 */
class Entity {
public:
    explicit Entity(std::string name);

    /**
     * @brief Constructs and attaches a component of type T.
     *
     * @tparam T    A type that derives from Component.
     * @tparam Args Constructor argument types forwarded to T.
     * @return Reference to the newly created component.
     */
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>,
                      "T must derive from Component");
        auto c  = std::make_unique<T>(std::forward<Args>(args)...);
        c->entity = this;
        T& ref  = *c;
        m_components.push_back(std::move(c));
        return ref;
    }

    /**
     * @brief Finds the first component of type T attached to this entity.
     * @return Pointer to the component, or nullptr if not found.
     */
    template<typename T>
    T* getComponent() {
        for (auto& c : m_components)
            if (auto* p = dynamic_cast<T*>(c.get())) return p;
        return nullptr;
    }

    /** @brief Const overload of getComponent(). */
    template<typename T>
    const T* getComponent() const {
        for (const auto& c : m_components)
            if (const auto* p = dynamic_cast<const T*>(c.get())) return p;
        return nullptr;
    }

    /** @brief Returns true if a component of type T is attached to this entity. */
    template<typename T>
    bool hasComponent() const { return getComponent<T>() != nullptr; }

    /** @brief Calls onUpdate(dt) on all components (skipped when active == false). */
    void onUpdate(float dt);

    /** @brief Calls onRender() on all components. */
    void onRender();

    std::string name;          ///< Human-readable identifier; not required to be unique.
    bool        active = true; ///< When false, this entity is skipped during update.

private:
    std::vector<std::unique_ptr<Component>> m_components;
};

} // namespace forge
