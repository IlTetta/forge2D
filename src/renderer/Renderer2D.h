#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace forge {

class Texture;

/**
 * @brief Stateless batch renderer for 2D quads.
 *
 * All geometry submitted between beginScene() and endScene() is batched into
 * a single vertex buffer and flushed in the minimum number of draw calls.
 *
 * **Lifecycle:**
 * 1. Call init() once at startup.
 * 2. Each frame: beginScene() → draw calls → endScene().
 * 3. Call shutdown() when the application exits.
 *
 * **Coordinate system:** world-space Y increases upward (OpenGL convention).
 * Position arguments refer to the **centre** of the quad.
 *
 * @code
 * forge::Renderer2D::beginScene(camera.getViewProjection());
 *
 * forge::Renderer2D::drawQuad({0.f, 0.f}, {64.f, 64.f}, playerTexture);
 * forge::Renderer2D::drawQuad({200.f, 0.f}, {32.f, 32.f}, {1.f, 0.f, 0.f, 1.f});
 *
 * forge::Renderer2D::endScene();
 * @endcode
 */
class Renderer2D {
public:
    /** @brief Initialises the batch renderer (VBO, IBO, shader, white texture). */
    static void init();

    /** @brief Releases all GPU resources. Call before the OpenGL context is destroyed. */
    static void shutdown();

    /**
     * @brief Begins a new frame. Must be called before any draw calls.
     * @param viewProjection The camera's combined view-projection matrix.
     */
    static void beginScene(const glm::mat4& viewProjection);

    /** @brief Flushes remaining geometry and ends the frame. */
    static void endScene();

    /** @brief Flushes the current batch immediately (called automatically when the batch is full). */
    static void flush();

    // ---- Solid-colour quads ------------------------------------------------

    /**
     * @brief Draws a solid-colour axis-aligned quad.
     * @param position Centre in world space.
     * @param size     Width and height in world units.
     * @param color    RGBA colour.
     */
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const glm::vec4& color);

    /**
     * @brief Draws a textured axis-aligned quad.
     * @param position Centre in world space.
     * @param size     Width and height in world units.
     * @param texture  Source texture.
     * @param tint     Multiplicative RGBA tint (default = white / no tint).
     */
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         const Texture& texture,
                         const glm::vec4& tint = glm::vec4(1.0f));

    // ---- Rotated quads -----------------------------------------------------

    /**
     * @brief Draws a solid-colour rotated quad.
     * @param rotationDeg Rotation in degrees, counter-clockwise, around the quad centre.
     */
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         float rotationDeg, const glm::vec4& color);

    /** @brief Draws a textured rotated quad. */
    static void drawQuad(const glm::vec2& position, const glm::vec2& size,
                         float rotationDeg, const Texture& texture,
                         const glm::vec4& tint = glm::vec4(1.0f));

    // ---- Sub-texture (spritesheet / tilemap) --------------------------------

    /**
     * @brief Draws a region of a texture defined by a UV rect.
     * @param uvRect UV sub-rectangle {u0, v0, u1, v1} in [0, 1].
     *               Use AnimationController::getCurrentUV() or TileMap internals.
     */
    static void drawSubTexture(const glm::vec2& position, const glm::vec2& size,
                               const Texture& texture, const glm::vec4& uvRect,
                               const glm::vec4& tint = glm::vec4(1.0f));

    /** @brief Rotated sub-texture overload. */
    static void drawSubTexture(const glm::vec2& position, const glm::vec2& size,
                               float rotationDeg, const Texture& texture,
                               const glm::vec4& uvRect,
                               const glm::vec4& tint = glm::vec4(1.0f));

    // ---- Diagnostics -------------------------------------------------------

    /** @brief Per-frame rendering statistics. */
    struct Stats {
        uint32_t drawCalls = 0; ///< Number of glDrawElements calls issued this frame.
        uint32_t quadCount = 0; ///< Total quads submitted this frame.
    };

    /** @brief Returns the statistics accumulated since the last resetStats() call. */
    static Stats getStats();

    /** @brief Resets draw-call and quad counters to zero. */
    static void resetStats();
};

} // namespace forge
