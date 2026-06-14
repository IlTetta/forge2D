#pragma once
#include "Entity.h"
#include "../physics/PhysicsWorld.h"
#include <memory>
#include <string>
#include <vector>

namespace forge {

/**
 * @brief Container for entities and the owner of the physics world.
 *
 * Subclass Scene and override the lifecycle hooks to build your level:
 * - onLoad()   — spawn entities, load assets
 * - onUnload() — release scene-specific resources
 * - onUpdate() — per-frame logic (call Scene::onUpdate(dt) to tick components + physics)
 * - onRender() — draw calls (call Scene::onRender() to draw all entities)
 *
 * Transitions between scenes are managed by SceneManager.
 *
 * @code
 * class GameScene : public forge::Scene {
 * public:
 *     GameScene() : forge::Scene("Game") {}
 *
 *     void onLoad() override {
 *         auto& player = createEntity("Player");
 *         player.addComponent<forge::Rigidbody2D>();
 *     }
 *
 *     void onUpdate(float dt) override {
 *         // custom logic here ...
 *         Scene::onUpdate(dt); // must call base to tick physics
 *     }
 * };
 * @endcode
 */
class Scene {
public:
    explicit Scene(std::string name);
    virtual ~Scene() = default;

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;

    /** @brief Called once when the scene becomes active. Populate entities here. */
    virtual void onLoad()   {}

    /** @brief Called once when the scene is about to be replaced. Release assets here. */
    virtual void onUnload() {}

    /**
     * @brief Ticks all active entities and steps the physics world.
     *
     * Always call the base implementation when overriding to keep physics running.
     * @param dt Delta time in seconds.
     */
    virtual void onUpdate(float dt);

    /** @brief Calls onRender() on every entity in the scene. */
    virtual void onRender();

    /**
     * @brief Creates a new entity and automatically attaches a Transform.
     * @param name Identifier used by findEntity() and log messages.
     * @return Reference to the new entity.
     */
    Entity& createEntity(const std::string& name = "Entity");

    /**
     * @brief Removes the first entity whose name matches @p name.
     *
     * Safe to call outside of the entity iteration (deferred internally).
     */
    void destroyEntity(const std::string& name);

    /** @brief Returns a pointer to the first entity with the given name, or nullptr. */
    Entity*            findEntity(const std::string& name);

    /** @brief Returns the scene's name as passed to the constructor. */
    const std::string& getName() const { return m_name; }

protected:
    std::vector<std::unique_ptr<Entity>> m_entities; ///< All entities owned by this scene.

private:
    std::string  m_name;
    PhysicsWorld m_physicsWorld;
};

} // namespace forge
