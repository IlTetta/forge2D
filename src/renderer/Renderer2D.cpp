#include "Renderer2D.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <memory>
#include <string>

namespace forge {

// ---- constants -----------------------------------------------------------
static constexpr uint32_t MAX_QUADS         = 10000;
static constexpr uint32_t MAX_VERTICES      = MAX_QUADS * 4;
static constexpr uint32_t MAX_INDICES       = MAX_QUADS * 6;
static constexpr uint32_t MAX_TEXTURE_SLOTS = 16;

// ---- vertex layout -------------------------------------------------------
struct QuadVertex {
    glm::vec2 position;   // location 0
    glm::vec2 texcoord;   // location 1
    glm::vec4 color;      // location 2
    float     texIndex;   // location 3
};

// ---- renderer state ------------------------------------------------------
struct R2DData {
    uint32_t vao = 0;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer>  indexBuffer;
    std::unique_ptr<Shader>       shader;
    std::unique_ptr<Texture>      whiteTexture;

    // CPU-side vertex staging buffer
    QuadVertex  vertexBufferBase[MAX_VERTICES];
    QuadVertex* vertexBufferPtr = nullptr;
    uint32_t    quadCount       = 0;

    // Texture slot table  (slot 0 = white, slots 1-15 = user textures)
    std::array<const Texture*, MAX_TEXTURE_SLOTS> texSlots{};
    uint32_t texSlotIndex = 1;

    glm::mat4 viewProjection{1.0f};

    Renderer2D::Stats stats;
};
static R2DData s_data;

// --------------------------------------------------------------------------

void Renderer2D::init() {
    // Build a shared index buffer for all quads (pattern: 0,1,2, 2,3,0 per quad)
    uint32_t indices[MAX_INDICES];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < MAX_INDICES; i += 6) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glGenVertexArrays(1, &s_data.vao);
    glBindVertexArray(s_data.vao);

    s_data.vertexBuffer = std::make_unique<VertexBuffer>(MAX_VERTICES * sizeof(QuadVertex));
    s_data.vertexBuffer->bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, texcoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, color));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                          (void*)offsetof(QuadVertex, texIndex));

    // IndexBuffer constructor binds GL_ELEMENT_ARRAY_BUFFER — captured by the VAO
    s_data.indexBuffer = std::make_unique<IndexBuffer>(indices, MAX_INDICES);

    glBindVertexArray(0);

    s_data.shader = std::make_unique<Shader>(
        FORGE_ASSETS_DIR "/shaders/basic.vert",
        FORGE_ASSETS_DIR "/shaders/basic.frag"
    );
    s_data.shader->bind();

    // Bind each sampler uniform to its matching texture unit (done once at init)
    for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; ++i)
        s_data.shader->setInt("u_textures[" + std::to_string(i) + "]", (int)i);

    // Slot 0: 1x1 opaque white — used for solid-color quads
    s_data.whiteTexture = std::make_unique<Texture>(1, 1, 0xFFFFFFFFu);
    s_data.texSlots[0]  = s_data.whiteTexture.get();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FG_INFO("Renderer2D initialised  (batch, MAX_QUADS=%u, TEX_SLOTS=%u)",
            MAX_QUADS, MAX_TEXTURE_SLOTS);
}

void Renderer2D::shutdown() {
    s_data.shader.reset();
    s_data.whiteTexture.reset();
    s_data.indexBuffer.reset();
    s_data.vertexBuffer.reset();
    glDeleteVertexArrays(1, &s_data.vao);
    FG_INFO("Renderer2D shut down");
}

// --------------------------------------------------------------------------

void Renderer2D::beginScene(const glm::mat4& viewProjection) {
    s_data.viewProjection  = viewProjection;
    s_data.vertexBufferPtr = s_data.vertexBufferBase;
    s_data.quadCount       = 0;
    s_data.texSlotIndex    = 1; // slot 0 (white) is permanent
}

void Renderer2D::flush() {
    if (s_data.quadCount == 0) return;

    // Upload only the filled portion of the staging buffer
    uint32_t dataSize = s_data.quadCount * 4 * sizeof(QuadVertex);
    s_data.vertexBuffer->setData(s_data.vertexBufferBase, dataSize);

    // Bind every active texture to its slot
    for (uint32_t i = 0; i < s_data.texSlotIndex; ++i)
        s_data.texSlots[i]->bind(i);

    s_data.shader->bind();
    s_data.shader->setMat4("u_viewProjection", s_data.viewProjection);

    glBindVertexArray(s_data.vao);
    glDrawElements(GL_TRIANGLES, (int)(s_data.quadCount * 6), GL_UNSIGNED_INT, nullptr);

    ++s_data.stats.drawCalls;
    s_data.stats.quadCount += s_data.quadCount;

    // Reset batch pointers (keep slot 0 = white)
    s_data.quadCount       = 0;
    s_data.vertexBufferPtr = s_data.vertexBufferBase;
    s_data.texSlotIndex    = 1;
}

void Renderer2D::endScene() {
    flush();
}

// --------------------------------------------------------------------------

// pos = bottom-left corner, rotRad = rotation in radians around the quad centre
// uv  = {u0, v0, u1, v1} sub-texture rect in [0,1]
static void pushQuad(const glm::vec2& pos, const glm::vec2& size,
                     float rotRad, const glm::vec4& color, float texIdx,
                     const glm::vec4& uv = {0.f, 0.f, 1.f, 1.f})
{
    const glm::vec2 half   = size * 0.5f;
    const glm::vec2 centre = pos + half;
    const float     c      = glm::cos(rotRad);
    const float     s      = glm::sin(rotRad);

    auto rotate = [&](glm::vec2 p) -> glm::vec2 {
        return { p.x * c - p.y * s + centre.x,
                 p.x * s + p.y * c + centre.y };
    };

    QuadVertex* v = s_data.vertexBufferPtr;
    v[0] = {rotate({-half.x, -half.y}), {uv.x, uv.y}, color, texIdx};
    v[1] = {rotate({ half.x, -half.y}), {uv.z, uv.y}, color, texIdx};
    v[2] = {rotate({ half.x,  half.y}), {uv.z, uv.w}, color, texIdx};
    v[3] = {rotate({-half.x,  half.y}), {uv.x, uv.w}, color, texIdx};
    s_data.vertexBufferPtr += 4;
    ++s_data.quadCount;
}

// Helper: resolve texture slot (shared by both textured drawQuad overloads)
static float resolveTexSlot(const Texture& tex) {
    for (uint32_t i = 1; i < s_data.texSlotIndex; ++i)
        if (s_data.texSlots[i] == &tex) return (float)i;

    if (s_data.texSlotIndex >= MAX_TEXTURE_SLOTS)
        Renderer2D::flush();

    float idx = (float)s_data.texSlotIndex;
    s_data.texSlots[s_data.texSlotIndex++] = &tex;
    return idx;
}

// ---- public drawQuad overloads -------------------------------------------

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,
                          const glm::vec4& color)
{
    drawQuad(pos, size, 0.f, color);
}

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,
                          const Texture& tex, const glm::vec4& tint)
{
    drawQuad(pos, size, 0.f, tex, tint);
}

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,
                          float rotDeg, const glm::vec4& color)
{
    if (s_data.quadCount >= MAX_QUADS) flush();
    pushQuad(pos, size, glm::radians(rotDeg), color, 0.f);
}

void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,
                          float rotDeg, const Texture& tex, const glm::vec4& tint)
{
    if (s_data.quadCount >= MAX_QUADS) flush();
    pushQuad(pos, size, glm::radians(rotDeg), tint, resolveTexSlot(tex));
}

void Renderer2D::drawSubTexture(const glm::vec2& pos, const glm::vec2& size,
                                const Texture& tex, const glm::vec4& uvRect,
                                const glm::vec4& tint)
{
    drawSubTexture(pos, size, 0.f, tex, uvRect, tint);
}

void Renderer2D::drawSubTexture(const glm::vec2& pos, const glm::vec2& size,
                                float rotDeg, const Texture& tex,
                                const glm::vec4& uvRect, const glm::vec4& tint)
{
    if (s_data.quadCount >= MAX_QUADS) flush();
    pushQuad(pos, size, glm::radians(rotDeg), tint, resolveTexSlot(tex), uvRect);
}

// --------------------------------------------------------------------------

Renderer2D::Stats Renderer2D::getStats() {
    return s_data.stats;
}

void Renderer2D::resetStats() {
    s_data.stats = {};
}

} // namespace forge
