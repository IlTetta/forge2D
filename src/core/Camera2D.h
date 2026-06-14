#pragma once
#include <glm/glm.hpp>

namespace forge {

/**
 * @brief Orthographic 2D camera with pan and zoom.
 *
 * Maintains a view-projection matrix for the batch renderer.
 * World-space Y increases upward (OpenGL convention).
 *
 * @code
 * forge::Camera2D cam(1280.f, 720.f);
 * cam.setPosition({100.f, 0.f});
 * cam.setZoom(1.5f);
 *
 * forge::Renderer2D::beginScene(cam.getViewProjection());
 * // ... draw calls ...
 * forge::Renderer2D::endScene();
 * @endcode
 */
class Camera2D {
public:
    /**
     * @brief Constructs the camera with the given viewport size at zoom 1.
     * @param width  Viewport width in world units.
     * @param height Viewport height in world units.
     */
    Camera2D(float width, float height);

    /** @brief Moves the camera to an absolute world-space position. */
    void setPosition(const glm::vec2& pos);

    /** @brief Translates the camera by @p delta world units. */
    void move(const glm::vec2& delta);

    /** @brief Sets the zoom factor (>1 = zoomed in, <1 = zoomed out). */
    void setZoom(float zoom);

    /** @brief Multiplies the current zoom by @p factor. */
    void adjustZoom(float factor);

    /** @brief Returns the current world-space position of the camera centre. */
    const glm::vec2& getPosition() const { return m_position; }

    /** @brief Returns the current zoom factor. */
    float getZoom() const { return m_zoom; }

    /** @brief Returns the combined view-projection matrix for the shader. */
    const glm::mat4& getViewProjection() const { return m_viewProjection; }

    /**
     * @brief Converts a screen-space point to world space.
     * @param screenPos    Position in pixels, origin at top-left.
     * @param viewportSize Viewport dimensions in pixels.
     */
    glm::vec2 screenToWorld(const glm::vec2& screenPos,
                            const glm::vec2& viewportSize) const;

private:
    void recalculate();

    glm::mat4 m_projection{1.f};
    glm::mat4 m_view{1.f};
    glm::mat4 m_viewProjection{1.f};

    glm::vec2 m_position{0.f};
    float     m_zoom = 1.f;
};

} // namespace forge
