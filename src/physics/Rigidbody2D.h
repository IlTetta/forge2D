#pragma once
#include "../scene/Component.h"
#include <glm/glm.hpp>

namespace forge {

/**
 * @brief Applies gravity and integrates velocity into the entity's Transform.
 *
 * Adding Rigidbody2D to an entity makes it dynamic: each frame onUpdate()
 * applies gravity (scaled by @ref gravityScale) to @ref velocity.y and then
 * moves the entity by `velocity * dt`. Collision resolution is handled by
 * PhysicsWorld, which modifies velocity and position after detection.
 *
 * Set @ref isKinematic to true to disable gravity and drive the entity purely
 * by setting velocity or Transform::position directly (useful for moving platforms).
 *
 * @code
 * auto& rb = entity.addComponent<forge::Rigidbody2D>();
 * rb.gravityScale = 0.5f; // floaty feel
 *
 * // Instant jump:
 * rb.velocity.y = 400.f;
 *
 * // Impulse-based:
 * rb.applyImpulse({0.f, 400.f});
 * @endcode
 */
class Rigidbody2D : public Component {
public:
    glm::vec2 velocity{0.f, 0.f}; ///< Current velocity in world units per second.
    float     gravityScale = 1.f; ///< Multiplier applied to kGravity each frame.
    bool      isKinematic  = false; ///< If true, gravity is not applied.

    /** @brief Global gravity acceleration in pixels · s⁻² (negative = downward). */
    static constexpr float kGravity = -500.f;

    /** @brief Applies gravity and integrates velocity into Transform::position. */
    void onUpdate(float dt) override;

    /**
     * @brief Instantly adds @p impulse to the current velocity.
     * @param impulse Velocity change in world units per second (e.g. {0, 400} for a jump).
     */
    void applyImpulse(glm::vec2 impulse) { velocity += impulse; }
};

} // namespace forge
