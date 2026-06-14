#include "BoxCollider.h"
#include "../scene/Entity.h"
#include "../scene/Transform.h"

namespace forge {

AABB BoxCollider::getWorldAABB() const {
    glm::vec2 centre = offset;
    if (entity) {
        if (auto* t = entity->getComponent<Transform>())
            centre += t->position;
    }
    const glm::vec2 half = size * 0.5f;
    return {centre - half, centre + half};
}

} // namespace forge
