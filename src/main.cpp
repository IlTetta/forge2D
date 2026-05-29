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

        // Uncomment to load a texture from file:
        // m_texture = std::make_unique<forge::Texture>(FORGE_ASSETS_DIR "/textures/my_sprite.png");
    }

    ~ForgeApp() {
        forge::Renderer2D::shutdown();
    }

protected:
    void onUpdate(float /*dt*/) override {}

    void onRender() override {
        forge::Renderer2D::beginScene(m_proj);

        // red-ish quad (solid color)
        forge::Renderer2D::drawQuad({100.0f, 260.0f}, {300.0f, 200.0f}, {0.8f, 0.2f, 0.3f, 1.0f});

        // semi-transparent blue quad
        forge::Renderer2D::drawQuad({350.0f, 310.0f}, {300.0f, 200.0f}, {0.2f, 0.4f, 0.9f, 0.7f});

        // white quad (textured quad with white tint — replace with m_texture when ready)
        forge::Renderer2D::drawQuad({700.0f, 260.0f}, {200.0f, 200.0f}, {1.0f, 1.0f, 1.0f, 1.0f});

        forge::Renderer2D::endScene();
    }

private:
    glm::mat4 m_proj{1.0f};
    // std::unique_ptr<forge::Texture> m_texture;
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
