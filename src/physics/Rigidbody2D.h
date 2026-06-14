#pragma once
#include "../scene/Component.h"
#include <glm/glm.hpp>

namespace forge {

class Rigidbody2D : public Component {
public:
    glm::vec2 velocity{0.f, 0.f};
    float     gravityScale = 1.f;
    bool      isKinematic  = false; // ignores gravity; moves purely by velocity

    static constexpr float kGravity = -500.f; // pixels · s⁻²

    void onUpdate(float dt) override;
    void applyImpulse(glm::vec2 impulse) { velocity += impulse; }
};

} // namespace forge
