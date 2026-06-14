#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace forge {

class Texture;

/**
 * @brief Component that draws a textured or solid-colour quad at the entity's Transform.
 *
 * The quad is centred on Transform::position and sized by @ref size (in world units).
 * If @ref texture is nullptr the quad is filled with @ref color; otherwise the texture
 * is rendered with @ref color applied as a tint.
 *
 * SpriteRenderer respects Transform::scale (negative X flips horizontally) and
 * Transform::rotation.
 *
 * @code
 * auto& sr = entity.addComponent<forge::SpriteRenderer>();
 * sr.texture = myTexture.get();
 * sr.size    = {64.f, 64.f};
 * sr.color   = {1.f, 1.f, 1.f, 1.f}; // no tint
 * @endcode
 */
class SpriteRenderer : public Component {
public:
    glm::vec2      size{100.f, 100.f}; ///< Width and height in world units.
    glm::vec4      color{1.f};         ///< RGBA tint (white = no tint, alpha controls transparency).
    const Texture* texture = nullptr;  ///< Source texture; nullptr renders a solid colour quad.

    /** @brief Issues a draw call to Renderer2D using the owning entity's Transform. */
    void onRender() override;
};

} // namespace forge
