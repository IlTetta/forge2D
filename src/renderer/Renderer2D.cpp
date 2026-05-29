#include "Renderer2D.h"
#include "Shader.h"
#include "Texture.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <memory>

namespace forge {

struct QuadVertex {
    glm::vec2 position;
    glm::vec2 texcoord;
    glm::vec4 color;
};

struct R2DData {
    uint32_t vao = 0, vbo = 0, ebo = 0;
    std::unique_ptr<Shader>  shader;
    std::unique_ptr<Texture> whiteTexture;
    glm::mat4 viewProjection{1.0f};
};
static R2DData s_data;

// ---- internal helpers (internal linkage, full access to forge types) ------
namespace {

void submitQuad(const glm::vec2& pos, const glm::vec2& size,
                const Texture& tex, const glm::vec4& color)
{
    const std::array<QuadVertex, 4> verts = {{
        {{ pos.x,          pos.y          }, {0.0f, 0.0f}, color},
        {{ pos.x + size.x, pos.y          }, {1.0f, 0.0f}, color},
        {{ pos.x + size.x, pos.y + size.y }, {1.0f, 1.0f}, color},
        {{ pos.x,          pos.y + size.y }, {0.0f, 1.0f}, color}
    }};

    glBindVertexArray(s_data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_data.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts.data());

    s_data.shader->bind();
    s_data.shader->setMat4("u_viewProjection", s_data.viewProjection);

    tex.bind(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

} // anonymous namespace
// --------------------------------------------------------------------------

void Renderer2D::init() {
    constexpr uint32_t indices[] = {0, 1, 2,  2, 3, 0};

    glGenVertexArrays(1, &s_data.vao);
    glGenBuffers(1, &s_data.vbo);
    glGenBuffers(1, &s_data.ebo);

    glBindVertexArray(s_data.vao);

    glBindBuffer(GL_ARRAY_BUFFER, s_data.vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_data.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, texcoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, color));

    glBindVertexArray(0);

    s_data.shader = std::make_unique<Shader>(
        FORGE_ASSETS_DIR "/shaders/basic.vert",
        FORGE_ASSETS_DIR "/shaders/basic.frag"
    );
    s_data.shader->bind();
    s_data.shader->setInt("u_texture", 0);

    // 1x1 opaque white — used for solid-color quads
    s_data.whiteTexture = std::make_unique<Texture>(1, 1, 0xFFFFFFFFu);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FG_INFO("Renderer2D initialised");
}

void Renderer2D::shutdown() {
    s_data.shader.reset();
    s_data.whiteTexture.reset();
    glDeleteBuffers(1, &s_data.ebo);
    glDeleteBuffers(1, &s_data.vbo);
    glDeleteVertexArrays(1, &s_data.vao);
    FG_INFO("Renderer2D shut down");
}

void Renderer2D::beginScene(const glm::mat4& viewProjection) {
    s_data.viewProjection = viewProjection;
}

void Renderer2D::endScene() {}

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color) {
    submitQuad(pos, size, *s_data.whiteTexture, color);
}

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,
                          const Texture& tex, glm::vec4 tint) {
    submitQuad(pos, size, tex, tint);
}

} // namespace forge
