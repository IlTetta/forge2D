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

    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const Texture& texture,
                         const glm::vec4& tint = glm::vec4(1.0f));

    struct Stats {
        uint32_t drawCalls = 0;
        uint32_t quadCount =0;
    };
    static Stats getStats();
    static void resetStats();
};

} // namespace forge
