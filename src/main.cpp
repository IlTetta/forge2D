#include "core/Application.h"
#include "core/Camera2D.h"
#include "core/Log.h"
#include "input/Input.h"
#include "renderer/Renderer2D.h"
#include "renderer/TileMap.h"
#include "renderer/Texture.h"
#include "scene/AnimationController.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "scene/SpriteRenderer.h"
#include "scene/Transform.h"
#include <vector>

// ---------------------------------------------------------------------------
// Helpers — build programmatic textures without external assets
// ---------------------------------------------------------------------------
static std::unique_ptr<forge::Texture> makeSpritesheet() {
    // 4 frames × 32×32 px = 128×32 px strip (single row, 4 columns)
    // Colors are stored bottom-to-top in the buffer (matches OpenGL convention)
    constexpr int W = 128, H = 32, COLS = 4, FW = 32;
    std::vector<uint32_t> px(W * H);
    const uint32_t colors[COLS] = {
        0xFF4444CCu,  // frame 0 — red-ish   (ABGR)
        0xFF44CC44u,  // frame 1 — green
        0xFFCC4444u,  // frame 2 — blue
        0xFF44CCCCu,  // frame 3 — yellow
    };
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            px[y * W + x] = colors[x / FW];
    return std::make_unique<forge::Texture>(W, H, px.data());
}

static std::unique_ptr<forge::Texture> makeTileset() {
    // 4 tiles in a single row × 48×48 px = 192×48 px
    constexpr int W = 192, H = 48, COLS = 4, TW = 48;
    std::vector<uint32_t> px(W * H);
    const uint32_t colors[COLS] = {
        0xFF336699u,  // tile 1 — dark ground  (ABGR)
        0xFF33AA33u,  // tile 2 — grass
        0xFF886633u,  // tile 3 — water (blue-ish in ABGR = brownish here, relabelled "dirt")
        0xFF888888u,  // tile 4 — stone
    };
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            px[y * W + x] = colors[x / TW];
    return std::make_unique<forge::Texture>(W, H, px.data());
}

// ---------------------------------------------------------------------------
// Demo scene
// ---------------------------------------------------------------------------
class DemoScene : public forge::Scene {
public:
    DemoScene() : forge::Scene("Demo") {}

    void onLoad() override {
        // --- Spritesheet + AnimationController ---
        m_spritesheet = makeSpritesheet();

        auto& animated = createEntity("Animated Sprite");
        animated.getComponent<forge::Transform>()->position = {0.f, 80.f};
        auto& sr = animated.addComponent<forge::SpriteRenderer>();
        sr.size  = {80.f, 80.f};

        auto& ac = animated.addComponent<forge::AnimationController>();
        ac.setSpritesheet(m_spritesheet.get(), 32, 32);
        ac.addClip("cycle", forge::AnimationClip{{0, 1, 2, 3}, 4.f, true});
        ac.play("cycle");

        // --- Static reference entities ---
        auto& s1 = createEntity("Static A");
        s1.getComponent<forge::Transform>()->position = {-160.f, 80.f};
        s1.addComponent<forge::SpriteRenderer>().color = {0.8f, 0.8f, 0.8f, 1.f};

        auto& s2 = createEntity("Static B");
        s2.getComponent<forge::Transform>()->position = {160.f, 80.f};
        s2.addComponent<forge::SpriteRenderer>().color = {0.5f, 0.5f, 0.9f, 0.7f};

        // --- TileMap: 12×8 grid of 48×48-px tiles, centred at origin ---
        m_tileset = makeTileset();
        m_tileMap.setTileset(m_tileset.get(), 48, 48, 4);
        m_tileMap.resize(12, 8);

        // Simple map layout (row 0 = top, gid 1-4 = tile types)
        static constexpr int kMap[8][12] = {
            { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 },
            { 4, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 4 },
            { 4, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 4 },
            { 4, 2, 2, 3, 3, 1, 1, 3, 3, 2, 2, 4 },
            { 4, 2, 2, 3, 3, 1, 1, 3, 3, 2, 2, 4 },
            { 4, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 4 },
            { 4, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 4 },
            { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 },
        };
        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 12; ++c)
                m_tileMap.setTile(c, r, kMap[r][c]);
    }

    void onUpdate(float dt) override {
        Scene::onUpdate(dt);
    }

    void onRender() override {
        // Map is 12×8 tiles × 48px = 576×384 world units; centre at origin
        m_tileMap.render({-288.f, -192.f});
        Scene::onRender();
    }

private:
    std::unique_ptr<forge::Texture> m_spritesheet;
    std::unique_ptr<forge::Texture> m_tileset;
    forge::TileMap                  m_tileMap;
};

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------
class ForgeApp final : public forge::Application {
public:
    ForgeApp()
        : forge::Application({"forge2d", 1280, 720, false})
        , m_camera(1280.f, 720.f)
    {
        forge::Renderer2D::init();
        forge::SceneManager::setScene(std::make_unique<DemoScene>());
    }

    ~ForgeApp() { forge::Renderer2D::shutdown(); }

protected:
    void onUpdate(float dt) override {
        const float panSpeed = 400.f / m_camera.getZoom();
        if (forge::Input::isKeyDown(forge::Key::A) || forge::Input::isKeyDown(forge::Key::Left))
            m_camera.move({-panSpeed * dt, 0.f});
        if (forge::Input::isKeyDown(forge::Key::D) || forge::Input::isKeyDown(forge::Key::Right))
            m_camera.move({ panSpeed * dt, 0.f});
        if (forge::Input::isKeyDown(forge::Key::S) || forge::Input::isKeyDown(forge::Key::Down))
            m_camera.move({0.f, -panSpeed * dt});
        if (forge::Input::isKeyDown(forge::Key::W) || forge::Input::isKeyDown(forge::Key::Up))
            m_camera.move({0.f,  panSpeed * dt});

        float scroll = forge::Input::getScrollDelta();
        if (scroll != 0.f) m_camera.adjustZoom(1.f + scroll * 0.1f);

        if (forge::Input::isKeyPressed(forge::Key::R)) {
            m_camera.setPosition({0.f, 0.f});
            m_camera.setZoom(1.f);
        }

        forge::SceneManager::onUpdate(dt);
    }

    void onRender() override {
        forge::Renderer2D::resetStats();
        forge::Renderer2D::beginScene(m_camera.getViewProjection());
        forge::SceneManager::onRender();
        forge::Renderer2D::endScene();
    }

private:
    forge::Camera2D m_camera;
};

// ---------------------------------------------------------------------------
int main() {
    forge::Log::init(forge::LogLevel::Info);
    FG_INFO("Controls: WASD/arrows = pan | scroll = zoom | R = reset");

    try {
        ForgeApp app;
        app.run();
    } catch (const std::exception& e) {
        FG_FATAL("Unhandled exception: %s", e.what());
    }
}
