#pragma once
#include <memory>
#include <vector>

namespace forge {
class Entity;

class PhysicsWorld {
public:
    // Detects and resolves AABB collisions among all active entities.
    void step(float dt, const std::vector<std::unique_ptr<Entity>>& entities);
};

} // namespace forge
