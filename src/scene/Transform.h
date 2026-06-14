#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace forge {

/**
 * @brief Stores the world-space position, scale, and rotation of an entity.
 *
 * Transform is added automatically to every entity by Scene::createEntity().
 * Other components (SpriteRenderer, BoxCollider, etc.) read from Transform
 * to determine where to draw or collide.
 *
 * Rotation is expressed in degrees, counter-clockwise, around the Z axis.
 * Scale defaults to (1, 1); negative X flips the sprite horizontally.
 */
class Transform : public Component {
public:
    glm::vec2 position{0.f, 0.f}; ///< World-space centre of the entity.
    glm::vec2 scale{1.f, 1.f};    ///< Scale factor per axis; negative X mirrors the sprite.
    float     rotation = 0.f;     ///< Rotation in degrees, counter-clockwise.

    /**
     * @brief Returns the combined TRS matrix (Translate * Rotate * Scale).
     *
     * Suitable for passing directly to a shader or combining with a projection.
     */
    glm::mat4 getMatrix() const {
        return glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f))
             * glm::rotate(glm::mat4(1.f), glm::radians(rotation), {0.f, 0.f, 1.f})
             * glm::scale(glm::mat4(1.f), glm::vec3(scale, 1.f));
    }
};

} // namespace forge
