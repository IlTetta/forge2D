#pragma once
#include "AABB.h"
#include "../scene/Component.h"
#include <functional>
#include <glm/glm.hpp>

namespace forge {

struct CollisionInfo {
    Entity*   other;   // entity this collider touched
    glm::vec2 normal;  // unit vector pointing away from other
    float     depth;   // penetration depth in world units
};

class BoxCollider : public Component {
public:
    glm::vec2 size{32.f, 32.f};        // axis-aligned extents in world units
    glm::vec2 offset{0.f, 0.f};        // local offset from Transform position
    bool      isTrigger = false;        // trigger: fires callback but skips resolution
    uint32_t  layer     = 0x1u;        // bitmask layer this object is on
    uint32_t  mask      = 0xFFFFFFFFu; // bitmask of layers this object reacts to

    std::function<void(const CollisionInfo&)> onCollision;

    AABB getWorldAABB() const;
};

} // namespace forge
