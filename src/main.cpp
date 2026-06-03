#include "core/Application.h"
#include "core/Log.h"
#include "core/Camera2D.h"
#include "input/Input.h"
#include "renderer/Renderer2D.h"

class ForgeApp final : public forge::Application {
public:
    ForgeApp()
        : forge::Application({"forge2d", 1280, 720, false})
        , m_camera(1280.f, 720.f)
    {
        forge::Renderer2D::init();
    }

    ~ForgeApp() {
        forge::Renderer2D::shutdown();
    }

protected:
    void onUpdate(float dt) override {
        // Pan speed scales down when zoomed in so movement feels consistent
        const float panSpeed = 400.f / m_camera.getZoom();

        if (forge::Input::isKeyDown(forge::Key::A) || forge::Input::isKeyDown(forge::Key::Left))
            m_camera.move({-panSpeed * dt, 0.f});
        if (forge::Input::isKeyDown(forge::Key::D) || forge::Input::isKeyDown(forge::Key::Right))
            m_camera.move({ panSpeed * dt, 0.f});
        if (forge::Input::isKeyDown(forge::Key::S) || forge::Input::isKeyDown(forge::Key::Down))
            m_camera.move({0.f, -panSpeed * dt});
        if (forge::Input::isKeyDown(forge::Key::W) || forge::Input::isKeyDown(forge::Key::Up))
            m_camera.move({0.f,  panSpeed * dt});

        // Scroll wheel zooms — each notch is +/-10%
        float scroll = forge::Input::getScrollDelta();
        if (scroll != 0.f)
            m_camera.adjustZoom(1.f + scroll * 0.1f);

        // R resets camera
        if (forge::Input::isKeyPressed(forge::Key::R)) {
            m_camera.setPosition({0.f, 0.f});
            m_camera.setZoom(1.f);
            FG_INFO("Camera reset");
        }
    }

    void onRender() override {
        forge::Renderer2D::resetStats();
        forge::Renderer2D::beginScene(m_camera.getViewProjection());

        // 7x5 grid of quads, spaced 280x200 world units apart, centred at origin
        static constexpr glm::vec4 kColors[] = {
            {0.80f, 0.20f, 0.20f, 1.f},
            {0.20f, 0.75f, 0.25f, 1.f},
            {0.20f, 0.35f, 0.85f, 1.f},
            {0.85f, 0.75f, 0.10f, 1.f},
            {0.75f, 0.20f, 0.75f, 1.f},
            {0.15f, 0.75f, 0.80f, 1.f},
        };
        int ci = 0;
        for (int row = -2; row <= 2; ++row) {
            for (int col = -3; col <= 3; ++col) {
                glm::vec2 pos = {col * 280.f - 55.f, row * 200.f - 55.f};
                forge::Renderer2D::drawQuad(pos, {110.f, 110.f}, kColors[ci++ % 6]);
            }
        }

        // Semi-transparent white square at the world origin as a landmark
        forge::Renderer2D::drawQuad({-75.f, -75.f}, {150.f, 150.f}, {1.f, 1.f, 1.f, 0.12f});

        forge::Renderer2D::endScene();
    }

private:
    forge::Camera2D m_camera;
};

int main() {
    forge::Log::init(forge::LogLevel::Info);
    FG_INFO("Controls: WASD / arrow keys = pan | scroll = zoom | R = reset");

    try {
        ForgeApp app;
        app.run();
    } catch (const std::exception& e) {
        FG_FATAL("Unhandled exception: %s", e.what());
    }
}
