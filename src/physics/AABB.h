#pragma once
#include <glm/glm.hpp>
#include <algorithm>

namespace forge {

struct AABB {
    glm::vec2 min{0.f, 0.f};
    glm::vec2 max{0.f, 0.f};

    glm::vec2 centre() const { return (min + max) * 0.5f; }

    bool overlaps(const AABB& o) const {
        return min.x < o.max.x && max.x > o.min.x
            && min.y < o.max.y && max.y > o.min.y;
    }

    // Returns the MTV to move *this* out of `o` (zero if not overlapping).
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
