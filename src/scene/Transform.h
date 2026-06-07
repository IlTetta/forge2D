#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace forge {

class Transform : public Component {
public:
    glm::vec2 position{0.f, 0.f};  // world-space centre of the entity
    glm::vec2 scale{1.f, 1.f};
    float     rotation = 0.f;      // degrees, counter-clockwise

    // TRS matrix: translate * rotate * scale
    glm::mat4 getMatrix() const {
        return glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f))
             * glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.f, 0.f, 1.f})
             * glm::scale(glm::mat4(1.f), glm::vec3(scale, 1.f));
    }
};

} // namespace forge
