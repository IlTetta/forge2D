#pragma once
#include "Component.h"
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace forge {

class Entity {
public:
    explicit Entity(std::string name);

    // Add a component of type T, constructed with args.
    // Returns a reference to the new component.
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

    // Returns a pointer to the first component of type T, or nullptr.
    template<typename T>
    T* getComponent() {
        for (auto& c : m_components)
            if (auto* p = dynamic_cast<T*>(c.get())) return p;
        return nullptr;
    }

    template<typename T>
    const T* getComponent() const {
        for (const auto& c : m_components)
            if (const auto* p = dynamic_cast<const T*>(c.get())) return p;
        return nullptr;
    }

    template<typename T>
    bool hasComponent() const { return getComponent<T>() != nullptr; }

    void onUpdate(float dt);
    void onRender();

    std::string name;
    bool        active = true;

private:
    std::vector<std::unique_ptr<Component>> m_components;
};

} // namespace forge
