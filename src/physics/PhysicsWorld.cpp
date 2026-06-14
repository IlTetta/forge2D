#include "PhysicsWorld.h"
#include "BoxCollider.h"
#include "Rigidbody2D.h"
#include "../scene/Entity.h"
#include "../scene/Transform.h"
#include <glm/glm.hpp>

namespace forge {

void PhysicsWorld::step(float /*dt*/,
                        const std::vector<std::unique_ptr<Entity>>& entities)
{
    struct Entry { Entity* e; BoxCollider* bc; };
    std::vector<Entry> colliders;
    colliders.reserve(entities.size());

    for (const auto& e : entities) {
        if (!e->active) continue;
        if (auto* bc = e->getComponent<BoxCollider>())
            colliders.push_back({e.get(), bc});
    }

    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            auto& a = colliders[i];
            auto& b = colliders[j];

            // Layer mask check (bidirectional)
            if (!(a.bc->mask & b.bc->layer)) continue;
            if (!(b.bc->mask & a.bc->layer)) continue;

            AABB aabbA = a.bc->getWorldAABB();
            AABB aabbB = b.bc->getWorldAABB();

            if (!aabbA.overlaps(aabbB)) continue;

            glm::vec2 pen   = aabbA.getPenetration(aabbB);
            float     depth = glm::length(pen);
            glm::vec2 norm  = (depth > 0.f) ? (pen / depth) : glm::vec2{0.f, 1.f};

            if (a.bc->onCollision) a.bc->onCollision({b.e,  norm, depth});
            if (b.bc->onCollision) b.bc->onCollision({a.e, -norm, depth});

            if (a.bc->isTrigger || b.bc->isTrigger) continue;

            auto* rbA = a.e->getComponent<Rigidbody2D>();
            auto* rbB = b.e->getComponent<Rigidbody2D>();
            auto* tA  = a.e->getComponent<Transform>();
            auto* tB  = b.e->getComponent<Transform>();

            const bool dynA = (rbA != nullptr);
            const bool dynB = (rbB != nullptr);
            if (!dynA && !dynB) continue;

            constexpr float kRestitution = 0.2f;

            if (dynA && dynB) {
                if (tA) tA->position += pen * 0.5f;
                if (tB) tB->position -= pen * 0.5f;

                float relVel = glm::dot(rbA->velocity - rbB->velocity, norm);
                if (relVel < 0.f) {
                    float j = -(1.f + kRestitution) * relVel * 0.5f;
                    rbA->velocity += j * norm;
                    rbB->velocity -= j * norm;
                }
            } else if (dynA) {
                if (tA) tA->position += pen;
                float vDotN = glm::dot(rbA->velocity, norm);
                if (vDotN < 0.f)
                    rbA->velocity -= (1.f + kRestitution) * vDotN * norm;
            } else {
                if (tB) tB->position -= pen;
                float vDotN = glm::dot(rbB->velocity, -norm);
                if (vDotN < 0.f)
                    rbB->velocity -= (1.f + kRestitution) * vDotN * (-norm);
            }
        }
    }
}

} // namespace forge
