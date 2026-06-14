#pragma once
#include "Entity.h"
#include "../physics/PhysicsWorld.h"
#include <memory>
#include <string>
#include <vector>

namespace forge {

class Scene {
public:
    explicit Scene(std::string name);
    virtual ~Scene() = default;

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;

    // Override to populate entities and set up the scene.
    virtual void onLoad()   {}
    // Override to release scene-specific resources.
    virtual void onUnload() {}

    virtual void onUpdate(float dt);
    virtual void onRender();

    // Creates an entity, adds a Transform, and returns a reference to it.
    Entity& createEntity(const std::string& name = "Entity");

    // Removes the first entity whose name matches. Safe to call outside iteration.
    void destroyEntity(const std::string& name);

    Entity*       findEntity(const std::string& name);
    const std::string& getName() const { return m_name; }

protected:
    std::vector<std::unique_ptr<Entity>> m_entities;

private:
    std::string  m_name;
    PhysicsWorld m_physicsWorld;
};

} // namespace forge
