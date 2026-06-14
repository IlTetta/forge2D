#pragma once
#include <memory>
#include <vector>

namespace forge {
class Entity;

/**
 * @brief Detects and resolves AABB collisions between entities each frame.
 *
 * PhysicsWorld::step() is called automatically by Scene::onUpdate() — you
 * do not need to call it manually. The algorithm is O(n²) brute-force,
 * suitable for scenes with tens to low-hundreds of colliders.
 *
 * **Resolution:** For each overlapping solid pair, position correction is
 * applied along the axis of least penetration (MTV), then an impulse-based
 * velocity response is computed with a small restitution coefficient (0.2).
 *
 * **Triggers:** Colliders with BoxCollider::isTrigger == true still fire
 * their onCollision callback but do not participate in resolution.
 *
 * **Layers:** A collision between A and B is only processed when
 * `(A.mask & B.layer) && (B.mask & A.layer)`.
 */
class PhysicsWorld {
public:
    /**
     * @brief Runs one physics step: detection, callbacks, and resolution.
     * @param dt       Delta time (currently unused — resolution is position-based).
     * @param entities All entities in the scene to consider.
     */
    void step(float dt, const std::vector<std::unique_ptr<Entity>>& entities);
};

} // namespace forge
