#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace forge {

class Texture;

class Renderer2D {
public:
    static void init();
    static void shutdown();

    static void beginScene(const glm::mat4& viewProjection);
    static void endScene();
    static void flush();

    // No rotation
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const Texture& texture,
                         const glm::vec4& tint = glm::vec4(1.0f));

    // With rotation (degrees, counter-clockwise, around quad centre)
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         float rotationDeg, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         float rotationDeg, const Texture& texture,
                         const glm::vec4& tint = glm::vec4(1.0f));

    // Sub-texture: UV rect {u0, v0, u1, v1} in [0,1] — for spritesheets and tilemaps
    static void drawSubTexture(const glm::vec2& position, const glm::vec2& size,
                               const Texture& texture, const glm::vec4& uvRect,
                               const glm::vec4& tint = glm::vec4(1.0f));
    static void drawSubTexture(const glm::vec2& position, const glm::vec2& size,
                               float rotationDeg, const Texture& texture,
                               const glm::vec4& uvRect,
                               const glm::vec4& tint = glm::vec4(1.0f));

    struct Stats {
        uint32_t drawCalls = 0;
        uint32_t quadCount =0;
    };
    static Stats getStats();
    static void resetStats();
};

} // namespace forge
