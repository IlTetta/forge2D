#pragma once
#include <glm/glm.hpp>
#include <algorithm>

namespace forge {

/**
 * @brief Axis-Aligned Bounding Box used for collision detection.
 *
 * Defined by its minimum and maximum corners in world space. Used internally
 * by BoxCollider and PhysicsWorld; you rarely need to construct one directly.
 */
struct AABB {
    glm::vec2 min{0.f, 0.f}; ///< Bottom-left corner in world space.
    glm::vec2 max{0.f, 0.f}; ///< Top-right corner in world space.

    /** @brief Returns the centre point of the box. */
    glm::vec2 centre() const { return (min + max) * 0.5f; }

    /**
     * @brief Returns true if this box overlaps with @p o (touching edges do not count).
     */
    bool overlaps(const AABB& o) const {
        return min.x < o.max.x && max.x > o.min.x
            && min.y < o.max.y && max.y > o.min.y;
    }

    /**
     * @brief Returns the Minimum Translation Vector to push *this* out of @p o.
     *
     * The returned vector points along the axis of least penetration.
     * Returns {0, 0} when the boxes do not overlap.
     */
    glm::vec2 getPenetration(const AABB& o) const {
        if (!overlaps(o)) return {0.f, 0.f};

        float ox = std::min(max.x, o.max.x) - std::max(min.x, o.min.x);
        float oy = std::min(max.y, o.max.y) - std::max(min.y, o.min.y);

        if (ox <= oy) {
            float s = (centre().x < o.centre().x) ? -1.f : 1.f;
            return {s * ox, 0.f};
        } else {
            float s = (centre().y < o.centre().y) ? -1.f : 1.f;
            return {0.f, s * oy};
        }
    }
};

} // namespace forge
