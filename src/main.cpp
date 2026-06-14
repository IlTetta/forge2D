#include "core/Application.h"
#include "core/Camera2D.h"
#include "core/Log.h"
#include "input/Input.h"
#include "physics/BoxCollider.h"
#include "physics/Rigidbody2D.h"
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
    constexpr int W = 128, H = 32, COLS = 4, FW = 32;
    std::vector<uint32_t> px(W * H);
    const uint32_t colors[COLS] = {
        0xFF4444CCu,
        0xFF44CC44u,
        0xFFCC4444u,
        0xFF44CCCCu,
    };
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            px[y * W + x] = colors[x / FW];
    return std::make_unique<forge::Texture>(W, H, px.data());
}

static std::unique_ptr<forge::Texture> makeTileset() {
    constexpr int W = 192, H = 48, COLS = 4, TW = 48;
    std::vector<uint32_t> px(W * H);
    const uint32_t colors[COLS] = {
        0xFF336699u,
        0xFF33AA33u,
        0xFF886633u,
        0xFF888888u,
    };
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            px[y * W + x] = colors[x / TW];
    return std::make_unique<forge::Texture>(W, H, px.data());
}

// ---------------------------------------------------------------------------
// Demo scene — includes week 7 physics demo
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

        // --- TileMap ---
        m_tileset = makeTileset();
        m_tileMap.setTileset(m_tileset.get(), 48, 48, 4);
        m_tileMap.resize(12, 8);

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

        // ---------------------------------------------------------------
        // Week 7 — Physics demo
        // Positioned in the right half of the screen (x > 300)
        // ---------------------------------------------------------------

        // Ground platform (static — no Rigidbody2D)
        auto& ground = createEntity("Ground");
        ground.getComponent<forge::Transform>()->position = {450.f, -250.f};
        auto& gsr = ground.addComponent<forge::SpriteRenderer>();
        gsr.size  = {400.f, 24.f};
        gsr.color = {0.55f, 0.37f, 0.20f, 1.f};
        auto& gcol   = ground.addComponent<forge::BoxCollider>();
        gcol.size    = {400.f, 24.f};

        // Mid-air platform (static)
        auto& shelf = createEntity("Shelf");
        shelf.getComponent<forge::Transform>()->position = {500.f, -80.f};
        auto& shsr = shelf.addComponent<forge::SpriteRenderer>();
        shsr.size  = {160.f, 16.f};
        shsr.color = {0.35f, 0.55f, 0.35f, 1.f};
        auto& shcol = shelf.addComponent<forge::BoxCollider>();
        shcol.size  = {160.f, 16.f};

        // Left wall (static)
        auto& wallL = createEntity("WallL");
        wallL.getComponent<forge::Transform>()->position = {255.f, -130.f};
        auto& wlsr = wallL.addComponent<forge::SpriteRenderer>();
        wlsr.size  = {14.f, 260.f};
        wlsr.color = {0.4f, 0.4f, 0.5f, 1.f};
        auto& wlcol = wallL.addComponent<forge::BoxCollider>();
        wlcol.size  = {14.f, 260.f};

        // Right wall (static)
        auto& wallR = createEntity("WallR");
        wallR.getComponent<forge::Transform>()->position = {645.f, -130.f};
        auto& wrsr = wallR.addComponent<forge::SpriteRenderer>();
        wrsr.size  = {14.f, 260.f};
        wrsr.color = {0.4f, 0.4f, 0.5f, 1.f};
        auto& wrcol = wallR.addComponent<forge::BoxCollider>();
        wrcol.size  = {14.f, 260.f};

        // Dynamic box (Rigidbody2D + BoxCollider)
        m_box = &createEntity("Box");
        spawnBox();

        // Trigger zone — logs a message when the box enters it
        auto& trigger = createEntity("Trigger");
        trigger.getComponent<forge::Transform>()->position = {450.f, -155.f};
        auto& trsr = trigger.addComponent<forge::SpriteRenderer>();
        trsr.size  = {120.f, 80.f};
        trsr.color = {1.f, 1.f, 0.f, 0.15f};
        auto& trcol     = trigger.addComponent<forge::BoxCollider>();
        trcol.size      = {120.f, 80.f};
        trcol.isTrigger = true;
        trcol.onCollision = [](const forge::CollisionInfo& info) {
            FG_INFO("Trigger hit by: %s", info.other->name.c_str());
        };
    }

    void onUpdate(float dt) override {
        Scene::onUpdate(dt);

        // Space resets the falling box
        if (forge::Input::isKeyPressed(forge::Key::Space))
            spawnBox();
    }

    void onRender() override {
        m_tileMap.render({-288.f, -192.f});
        Scene::onRender();
    }

private:
    void spawnBox() {
        if (!m_box) return;
        auto* t  = m_box->getComponent<forge::Transform>();
        auto* rb = m_box->getComponent<forge::Rigidbody2D>();
        auto* bc = m_box->getComponent<forge::BoxCollider>();
        auto* sr = m_box->getComponent<forge::SpriteRenderer>();

        if (!rb) {
            // First-time setup
            if (t) t->position = {450.f, 200.f};
            auto& newSr = m_box->addComponent<forge::SpriteRenderer>();
            newSr.size  = {40.f, 40.f};
            newSr.color = {0.9f, 0.2f, 0.2f, 1.f};
            auto& newBc = m_box->addComponent<forge::BoxCollider>();
            newBc.size  = {40.f, 40.f};
            m_box->addComponent<forge::Rigidbody2D>();
        } else {
            if (t)  t->position = {450.f, 200.f};
            if (rb) rb->velocity = {0.f, 0.f};
            (void)bc; (void)sr;
        }
    }

    std::unique_ptr<forge::Texture> m_spritesheet;
    std::unique_ptr<forge::Texture> m_tileset;
    forge::TileMap                  m_tileMap;
    forge::Entity*                  m_box = nullptr;
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
    FG_INFO("Controls: WASD/arrows = pan | scroll = zoom | R = reset camera | Space = respawn box");

    try {
        ForgeApp app;
        app.run();
    } catch (const std::exception& e) {
        FG_FATAL("Unhandled exception: %s", e.what());
    }
}
