#pragma once
#include <glm/glm.hpp>

namespace forge {

class Camera2D {
public:
    // width/height in world units — defines the visible area at zoom=1
    Camera2D(float width, float height);

    void setPosition(const glm::vec2& pos);
    void move(const glm::vec2& delta);

    void setZoom(float zoom);
    void adjustZoom(float factor); // zoom *= factor

    const glm::vec2& getPosition()       const { return m_position; }
    float            getZoom()           const { return m_zoom; }
    const glm::mat4& getViewProjection() const { return m_viewProjection; }

    // Convert a screen-space point (pixels, origin top-left) to world space
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
