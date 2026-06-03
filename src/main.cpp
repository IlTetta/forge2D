#include "core/Application.h"
#include "core/Log.h"
#include "renderer/Renderer2D.h"
#include "renderer/Texture.h"
#include <glm/gtc/matrix_transform.hpp>

class ForgeApp final : public forge::Application {
public:
    ForgeApp()
        : forge::Application({"forge2d", 1280, 720, false})
    {
        forge::Renderer2D::init();
        m_proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);

        // Uncomment to load textures from file:
        // m_tex1 = std::make_unique<forge::Texture>(FORGE_ASSETS_DIR "/textures/sprite1.png");
        // m_tex2 = std::make_unique<forge::Texture>(FORGE_ASSETS_DIR "/textures/sprite2.png");
    }

    ~ForgeApp() {
        forge::Renderer2D::shutdown();
    }

protected:
    void onUpdate(float /*dt*/) override {}

    void onRender() override {
        forge::Renderer2D::resetStats();
        forge::Renderer2D::beginScene(m_proj);

        // --- solid-color quads (all batched into one draw call) ---
        forge::Renderer2D::drawQuad({  50.f, 500.f}, {100.f, 100.f}, {0.8f, 0.2f, 0.3f, 1.0f});
        forge::Renderer2D::drawQuad({ 160.f, 500.f}, {100.f, 100.f}, {0.2f, 0.8f, 0.3f, 1.0f});
        forge::Renderer2D::drawQuad({ 270.f, 500.f}, {100.f, 100.f}, {0.2f, 0.3f, 0.8f, 1.0f});
        forge::Renderer2D::drawQuad({ 380.f, 500.f}, {100.f, 100.f}, {0.9f, 0.7f, 0.1f, 1.0f});

        // --- semi-transparent quads (alpha blending) ---
        forge::Renderer2D::drawQuad({ 100.f, 300.f}, {200.f, 150.f}, {0.8f, 0.2f, 0.3f, 1.0f});
        forge::Renderer2D::drawQuad({ 180.f, 340.f}, {200.f, 150.f}, {0.2f, 0.4f, 0.9f, 0.6f});
        forge::Renderer2D::drawQuad({ 260.f, 380.f}, {200.f, 150.f}, {0.1f, 0.9f, 0.4f, 0.4f});

        // --- textured quads (uncomment when textures are loaded) ---
        // forge::Renderer2D::drawQuad({700.f, 200.f}, {200.f, 200.f}, *m_tex1);
        // forge::Renderer2D::drawQuad({920.f, 200.f}, {200.f, 200.f}, *m_tex2, {1.f, 0.8f, 0.8f, 1.f});

        forge::Renderer2D::endScene();

        // Log stats once per second to see how many draw calls were used
        m_statsTimer += 1.0f / 60.0f; // approximate; replace with real dt if desired
        if (m_statsTimer >= 1.0f) {
            m_statsTimer = 0.0f;
            auto s = forge::Renderer2D::getStats();
            FG_TRACE("Batch stats | drawCalls: %u  quads: %u", s.drawCalls, s.quadCount);
        }
    }

private:
    glm::mat4 m_proj{1.0f};
    float     m_statsTimer = 0.0f;

    // std::unique_ptr<forge::Texture> m_tex1;
    // std::unique_ptr<forge::Texture> m_tex2;
};

int main() {
    forge::Log::init(forge::LogLevel::Trace);

    try {
        ForgeApp app;
        app.run();
    } catch (const std::exception& e) {
        FG_FATAL("Unhandled exception: %s", e.what());
    }
}
