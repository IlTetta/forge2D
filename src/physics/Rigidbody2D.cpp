#include "Rigidbody2D.h"
#include "../scene/Entity.h"
#include "../scene/Transform.h"

namespace forge {

void Rigidbody2D::onUpdate(float dt) {
    if (!entity) return;
    auto* t = entity->getComponent<Transform>();
    if (!t) return;

    if (!isKinematic)
        velocity.y += kGravity * gravityScale * dt;

    t->position += velocity * dt;
}

} // namespace forge
