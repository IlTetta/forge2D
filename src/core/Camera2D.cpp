#include "Camera2D.h"
#include <glm/gtc/matrix_transform.hpp>

namespace forge {

Camera2D::Camera2D(float width, float height)
    : m_projection(glm::ortho(-width * 0.5f,  width * 0.5f,
                              -height * 0.5f, height * 0.5f,
                              -1.f, 1.f))
{
    recalculate();
}

void Camera2D::setPosition(const glm::vec2& pos) {
    m_position = pos;
    recalculate();
}

void Camera2D::move(const glm::vec2& delta) {
    m_position += delta;
    recalculate();
}

void Camera2D::setZoom(float zoom) {
    m_zoom = glm::max(zoom, 0.05f);
    recalculate();
}

void Camera2D::adjustZoom(float factor) {
    setZoom(m_zoom * factor);
}

glm::vec2 Camera2D::screenToWorld(const glm::vec2& screenPos,
                                   const glm::vec2& viewportSize) const
{
    // Screen (0,0)=top-left → NDC [-1,1] (flip Y because screen Y is down)
    glm::vec4 ndc{
         (screenPos.x / viewportSize.x) * 2.f - 1.f,
        -(screenPos.y / viewportSize.y) * 2.f + 1.f,
        0.f, 1.f
    };
    glm::vec4 world = glm::inverse(m_viewProjection) * ndc;
    return { world.x, world.y };
}

void Camera2D::recalculate() {
    // view = scale(zoom) * translate(-position)
    // Result: a world point p is transformed to (p - position) * zoom
    // Visible range at zoom=1: [position - W/2, position + W/2]
    // Visible range at zoom=2: [position - W/4, position + W/4]  (zoomed in)
    m_view = glm::scale(glm::mat4(1.f), glm::vec3(m_zoom, m_zoom, 1.f))
           * glm::translate(glm::mat4(1.f), glm::vec3(-m_position, 0.f));

    m_viewProjection = m_projection * m_view;
}

} // namespace forge
