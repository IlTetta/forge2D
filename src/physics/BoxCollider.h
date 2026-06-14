#pragma once
#include "AABB.h"
#include "../scene/Component.h"
#include <functional>
#include <glm/glm.hpp>

namespace forge {

/**
 * @brief Data passed to an onCollision callback when a collision is detected.
 */
struct CollisionInfo {
    Entity*   other;  ///< The other entity involved in the collision.
    glm::vec2 normal; ///< Unit vector pointing away from @p other (toward this collider).
    float     depth;  ///< Penetration depth in world units.
};

/**
 * @brief Axis-aligned box collider component.
 *
 * Defines a rectangular collision shape centred on the entity's Transform
 * (offset by @ref offset). Works in conjunction with PhysicsWorld, which
 * runs the detection and resolution step every frame.
 *
 * **Layers and masks** — collisions only occur between pairs where
 * `(a.mask & b.layer) && (b.mask & a.layer)`. This lets you define
 * groups (e.g. player, enemies, world) that selectively ignore each other.
 *
 * **Triggers** — when @ref isTrigger is true the collider fires
 * @ref onCollision but skips physics resolution (no position correction
 * or impulse is applied).
 *
 * @code
 * auto& bc      = entity.addComponent<forge::BoxCollider>();
 * bc.size       = {32.f, 48.f};
 * bc.layer      = kLayerPlayer;
 * bc.mask       = kLayerWorld;
 * bc.onCollision = [](const forge::CollisionInfo& info) {
 *     if (info.normal.y > 0.5f) isGrounded = true;
 * };
 * @endcode
 */
class BoxCollider : public Component {
public:
    glm::vec2 size{32.f, 32.f};        ///< Extents of the collision box in world units.
    glm::vec2 offset{0.f, 0.f};        ///< Local offset applied on top of Transform::position.
    bool      isTrigger = false;        ///< If true, fires onCollision but skips resolution.
    uint32_t  layer     = 0x1u;        ///< Bitmask layer this collider belongs to.
    uint32_t  mask      = 0xFFFFFFFFu; ///< Bitmask of layers this collider reacts to.

    /**
     * @brief Optional callback invoked whenever a collision (or trigger overlap) is detected.
     *
     * Called during PhysicsWorld::step(), once per overlapping pair per frame.
     */
    std::function<void(const CollisionInfo&)> onCollision;

    /** @brief Computes the world-space AABB from the owning entity's Transform. */
    AABB getWorldAABB() const;
};

} // namespace forge
