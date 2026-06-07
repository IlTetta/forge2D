#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace forge {

class Texture;

class SpriteRenderer : public Component {
public:
    glm::vec2      size{100.f, 100.f};  // base size in world units
    glm::vec4      color{1.f};          // tint (white = no tint)
    const Texture* texture = nullptr;   // optional; nullptr = solid color

    void onRender() override;
};

} // namespace forge
