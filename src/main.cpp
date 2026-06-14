#include "core/Application.h"
#include "core/Camera2D.h"
#include "core/Log.h"
#include "input/Input.h"
#include "physics/BoxCollider.h"
#include "physics/Rigidbody2D.h"
#include "renderer/Renderer2D.h"
#include "renderer/Texture.h"
#include "scene/AnimationController.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "scene/SpriteRenderer.h"
#include "scene/Transform.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// PlatformerScene
//
// Mini platformer that exercises every engine system:
//   - Entity-Component system (player, platforms, coins, goal)
//   - Rigidbody2D + BoxCollider for movement and collision
//   - AnimationController for the player sprite
//   - Trigger colliders for coin pickup and level exit
//   - Camera2D following the player with lerp
//
// Controls: A/D or Left/Right = move | Space/Up = jump | R = restart
// ---------------------------------------------------------------------------

// Layers
static constexpr uint32_t kLayerWorld  = 0x1u;
static constexpr uint32_t kLayerPlayer = 0x2u;
static constexpr uint32_t kLayerCoin   = 0x4u;

// ---------------------------------------------------------------------------
// Procedural textures (no external assets needed)
// ---------------------------------------------------------------------------
static std::unique_ptr<forge::Texture> makePlayerSheet()
{
    // 4 frames x 32x48: idle (blue), run1 (lighter), run2 (darker), jump (white-blue)
    constexpr int FW = 32, FH = 48, FRAMES = 4;
    constexpr int W = FW * FRAMES, H = FH;
    std::vector<uint32_t> px(W * H);

    const uint32_t cols[FRAMES] = {
        0xFF3355CCu,   // idle — blue
        0xFF4477EEu,   // run 1 — lighter blue
        0xFF2244AAu,   // run 2 — darker blue
        0xFF88AAFFu,   // jump  — pale blue
    };
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            px[y * W + x] = cols[x / FW];

    return std::make_unique<forge::Texture>(W, H, px.data());
}

static std::unique_ptr<forge::Texture> makeSolidTexture(uint32_t color)
{
    std::vector<uint32_t> px(2 * 2, color);
    return std::make_unique<forge::Texture>(2, 2, px.data());
}

// ---------------------------------------------------------------------------
class PlatformerScene : public forge::Scene {
public:
    PlatformerScene() : forge::Scene("Platformer") {}

    void onLoad() override
    {
        m_playerSheet  = makePlayerSheet();
        m_texGround    = makeSolidTexture(0xFF4A3020u);  // dark brown
        m_texPlatform  = makeSolidTexture(0xFF3A6E3Au);  // green
        m_texCoin      = makeSolidTexture(0xFF22CCFFu);  // cyan
        m_texGoal      = makeSolidTexture(0xFF22EE55u);  // bright green
        m_texBg        = makeSolidTexture(0xFF1A1A2Eu);  // dark navy

        buildLevel();
        spawnPlayer();

        FG_INFO("=== PLATFORMER ===  A/D move | Space/Up jump | R restart");
        FG_INFO("Collect all %d coins and reach the EXIT!", m_totalCoins);
    }

    void onUpdate(float dt) override
    {
        if (!m_player) return;

        auto* rb = m_player->getComponent<forge::Rigidbody2D>();
        auto* t  = m_player->getComponent<forge::Transform>();

        // --- Horizontal movement ---
        const float vx = (forge::Input::isKeyDown(forge::Key::D) || forge::Input::isKeyDown(forge::Key::Right) ?  kMoveSpeed : 0.f)
                       + (forge::Input::isKeyDown(forge::Key::A) || forge::Input::isKeyDown(forge::Key::Left)  ? -kMoveSpeed : 0.f);
        if (rb) rb->velocity.x = vx;

        // --- Jump input buffer ---
        // If Space/Up is pressed, store the request for up to kJumpBufferTime seconds.
        // This way a press that arrives 1-2 frames before landing still fires the jump.
        if (forge::Input::isKeyPressed(forge::Key::Space) || forge::Input::isKeyPressed(forge::Key::Up))
            m_jumpBuffer = kJumpBufferTime;
        else
            m_jumpBuffer -= dt;

        // --- Coyote time ---
        // Allow jumping for a short window after walking off a ledge.
        if (m_isGrounded)
            m_coyoteTime = kCoyoteTime;
        else
            m_coyoteTime -= dt;

        // --- Consume buffered jump when the player can actually jump ---
        if (rb && m_jumpBuffer > 0.f && m_coyoteTime > 0.f) {
            rb->velocity.y = kJumpImpulse;
            m_jumpBuffer  = 0.f;
            m_coyoteTime  = 0.f;   // consume coyote so they can't double-jump
        }

        // --- Sprite direction ---
        if (t) {
            if      (vx > 0.f) t->scale.x =  1.f;
            else if (vx < 0.f) t->scale.x = -1.f;
        }

        // --- Animation ---
        auto* ac = m_player->getComponent<forge::AnimationController>();
        if (ac) {
            if      (!m_isGrounded)  ac->play("jump");
            else if (vx != 0.f)      ac->play("run");
            else                     ac->play("idle");
        }

        // --- Reset grounded flag; physics step will re-set it via callback ---
        m_isGrounded = false;

        // --- Step entities + physics (sets m_isGrounded via callback) ---
        Scene::onUpdate(dt);

        // --- Deferred coin hide ---
        for (const auto& name : m_toHide)
            hideCoin(name);
        m_toHide.clear();

        // --- Death plane ---
        if (t && t->position.y < kDeathY && !m_completed) {
            FG_WARN("Fell into the void! Restarting...");
            respawn();
        }

        // --- Restart ---
        if (forge::Input::isKeyPressed(forge::Key::R)) {
            FG_INFO("Restarting level...");
            respawn();
        }

        // --- Camera follow (lerp on X, fixed Y offset) ---
        if (t) {
            const glm::vec2 target = {t->position.x, t->position.y + 60.f};
            m_cameraPos = glm::mix(m_cameraPos, target, 8.f * dt);
        }
    }

    void onRender() override
    {
        Scene::onRender();
    }

    const glm::vec2& getCameraPos() const { return m_cameraPos; }
    bool             isCompleted()  const { return m_completed; }
    int              getCoins()     const { return m_coins; }
    int              getTotalCoins() const { return m_totalCoins; }

private:
    // -----------------------------------------------------------------------
    // Level construction
    // -----------------------------------------------------------------------
    void buildLevel()
    {
        // Background fill (static, no collider, drawn first via entity order)
        auto& bg = createEntity("Background");
        bg.getComponent<forge::Transform>()->position = {kLevelCenterX, 0.f};
        auto& bgSr = bg.addComponent<forge::SpriteRenderer>();
        bgSr.texture = m_texBg.get();
        bgSr.size    = {kLevelWidth + 200.f, 900.f};

        // Ground
        makePlatform("Ground", {kLevelCenterX, kGroundY}, {kLevelWidth, 30.f}, {0.42f, 0.28f, 0.12f, 1.f});

        // Left wall (stop player from going out of bounds)
        makePlatform("WallLeft",  {kLevelMinX - 15.f, kGroundY + 200.f}, {30.f, 460.f}, {0.35f, 0.25f, 0.10f, 1.f});
        // Right wall (past the goal so the player must hit the trigger)
        makePlatform("WallRight", {kLevelMaxX + 15.f, kGroundY + 200.f}, {30.f, 460.f}, {0.35f, 0.25f, 0.10f, 1.f});

        // Floating platforms  (x-center, y-center, width)
        struct PlatDef { float x, y, w; };
        constexpr PlatDef plats[] = {
            { -200.f, -185.f, 200.f },  // 1 — low hop
            {   80.f, -120.f, 170.f },  // 2 — step up
            {  300.f,  -60.f, 150.f },  // 3 — higher
            {  480.f, -140.f, 180.f },  // 4 — step down (gap forces jump)
            {  680.f,  -40.f, 150.f },  // 5 — step up again
            {  860.f,   40.f, 130.f },  // 6 — near goal
        };
        int idx = 0;
        for (const auto& p : plats) {
            makePlatform("Plat" + std::to_string(++idx),
                         {p.x, p.y}, {p.w, 20.f},
                         {0.22f, 0.54f, 0.22f, 1.f});
        }

        // Coins
        struct CoinDef { float x, y; };
        constexpr CoinDef coins[] = {
            { -200.f, -130.f },  // above plat 1
            {   80.f,  -60.f },  // above plat 2
            {  300.f,    0.f },  // above plat 3
            {  480.f,  -80.f },  // above plat 4
            {  860.f,  100.f },  // above plat 6
        };
        for (const auto& c : coins)
            makeCoin({c.x, c.y});

        // Goal — trigger at the far right platform
        makeGoal({1020.f, 20.f});
    }

    forge::Entity& makePlatform(const std::string& name,
                                 glm::vec2 pos, glm::vec2 size,
                                 glm::vec4 color)
    {
        auto& e  = createEntity(name);
        auto* t  = e.getComponent<forge::Transform>();
        t->position = pos;

        auto& sr  = e.addComponent<forge::SpriteRenderer>();
        sr.size   = size;
        sr.color  = color;

        auto& bc  = e.addComponent<forge::BoxCollider>();
        bc.size   = size;
        bc.layer  = kLayerWorld;
        bc.mask   = kLayerPlayer;

        return e;
    }

    void makeCoin(glm::vec2 pos)
    {
        ++m_totalCoins;
        const std::string name = "Coin" + std::to_string(m_totalCoins);
        auto& e  = createEntity(name);
        auto* t  = e.getComponent<forge::Transform>();
        t->position = pos;

        auto& sr  = e.addComponent<forge::SpriteRenderer>();
        sr.size   = {20.f, 20.f};
        sr.color  = {1.f, 0.85f, 0.f, 1.f};  // gold

        auto& bc      = e.addComponent<forge::BoxCollider>();
        bc.size       = {20.f, 20.f};
        bc.isTrigger  = true;
        bc.layer      = kLayerCoin;
        bc.mask       = kLayerPlayer;

        const std::string coinName = name;
        bc.onCollision = [this, coinName](const forge::CollisionInfo&) {
            collectCoin(coinName);
        };
    }

    void makeGoal(glm::vec2 pos)
    {
        // Tall platform under the goal flag
        makePlatform("GoalPlatform", {pos.x, kGroundY + 75.f}, {80.f, 180.f}, {0.15f, 0.45f, 0.15f, 1.f});

        // Flag pole top
        auto& flag = createEntity("GoalFlag");
        flag.getComponent<forge::Transform>()->position = {pos.x, pos.y + 30.f};
        auto& fsr  = flag.addComponent<forge::SpriteRenderer>();
        fsr.size   = {40.f, 40.f};
        fsr.color  = {0.1f, 0.95f, 0.35f, 1.f};

        // Trigger zone
        auto& trigger = createEntity("Goal");
        auto* t = trigger.getComponent<forge::Transform>();
        t->position = pos;

        auto& sr  = trigger.addComponent<forge::SpriteRenderer>();
        sr.size   = {60.f, 80.f};
        sr.color  = {0.1f, 0.95f, 0.35f, 0.25f};

        auto& bc      = trigger.addComponent<forge::BoxCollider>();
        bc.size       = {60.f, 80.f};
        bc.isTrigger  = true;
        bc.layer      = kLayerWorld;
        bc.mask       = kLayerPlayer;
        bc.onCollision = [this](const forge::CollisionInfo&) {
            if (!m_completed) {
                m_completed = true;
                FG_INFO("=== LEVEL COMPLETE! Coins: %d/%d ===", m_coins, m_totalCoins);
            }
        };
    }

    // -----------------------------------------------------------------------
    // Player
    // -----------------------------------------------------------------------
    void spawnPlayer()
    {
        m_player = &createEntity("Player");
        auto* t  = m_player->getComponent<forge::Transform>();
        t->position = kSpawnPos;

        auto& sr   = m_player->addComponent<forge::SpriteRenderer>();
        sr.size    = {kPlayerW, kPlayerH};
        sr.texture = m_playerSheet.get();

        auto& ac = m_player->addComponent<forge::AnimationController>();
        ac.setSpritesheet(m_playerSheet.get(), 32, 48);
        ac.addClip("idle", forge::AnimationClip{{0},       2.f,  true});
        ac.addClip("run",  forge::AnimationClip{{1, 2},    8.f,  true});
        ac.addClip("jump", forge::AnimationClip{{3},       4.f,  false});
        ac.play("idle");

        auto& rb = m_player->addComponent<forge::Rigidbody2D>();
        (void)rb;

        auto& bc  = m_player->addComponent<forge::BoxCollider>();
        bc.size   = {kPlayerW - 4.f, kPlayerH};
        bc.layer  = kLayerPlayer;
        bc.mask   = kLayerWorld | kLayerCoin;

        // Detect ground via upward collision normal
        bc.onCollision = [this](const forge::CollisionInfo& info) {
            if (info.normal.y > 0.5f)
                m_isGrounded = true;
        };

        m_cameraPos = kSpawnPos;
    }

    void respawn()
    {
        if (!m_player) return;
        auto* t  = m_player->getComponent<forge::Transform>();
        auto* rb = m_player->getComponent<forge::Rigidbody2D>();
        if (t)  t->position  = kSpawnPos;
        if (rb) rb->velocity = {0.f, 0.f};
        m_isGrounded = false;
        m_jumpBuffer = 0.f;
        m_coyoteTime = 0.f;
        m_completed  = false;
        m_coins      = 0;

        // Re-show all collected coins
        for (const auto& name : m_collectedCoins) {
            auto* e = findEntity(name);
            if (!e) continue;
            if (auto* sr = e->getComponent<forge::SpriteRenderer>())
                sr->color.a = 1.f;
            if (auto* bc = e->getComponent<forge::BoxCollider>())
                bc->mask = kLayerPlayer;
        }
        m_collectedCoins.clear();
    }

    void collectCoin(const std::string& name)
    {
        // Guard against double-fire within the same frame
        for (const auto& n : m_toHide)
            if (n == name) return;

        ++m_coins;
        m_toHide.push_back(name);
        FG_INFO("Coin collected! %d/%d", m_coins, m_totalCoins);
    }

    void hideCoin(const std::string& name)
    {
        auto* e = findEntity(name);
        if (!e) return;
        if (auto* sr = e->getComponent<forge::SpriteRenderer>())
            sr->color.a = 0.f;
        if (auto* bc = e->getComponent<forge::BoxCollider>())
            bc->mask = 0;          // won't collide anymore
        m_collectedCoins.push_back(name);
    }

    // -----------------------------------------------------------------------
    // State
    // -----------------------------------------------------------------------
    forge::Entity*                  m_player     = nullptr;
    bool                            m_isGrounded = false;
    float                           m_jumpBuffer = 0.f;   // seconds remaining on buffered jump
    float                           m_coyoteTime = 0.f;   // seconds remaining for coyote jump
    bool                            m_completed  = false;
    int                             m_coins      = 0;
    int                             m_totalCoins = 0;
    glm::vec2                       m_cameraPos      = {0.f, 0.f};
    std::vector<std::string>        m_toHide;         // coins to hide at end of frame
    std::vector<std::string>        m_collectedCoins; // hidden coins, restored on respawn

    std::unique_ptr<forge::Texture> m_playerSheet;
    std::unique_ptr<forge::Texture> m_texGround;
    std::unique_ptr<forge::Texture> m_texPlatform;
    std::unique_ptr<forge::Texture> m_texCoin;
    std::unique_ptr<forge::Texture> m_texGoal;
    std::unique_ptr<forge::Texture> m_texBg;

    // Level constants
    static constexpr float kGroundY      = -300.f;
    static constexpr float kDeathY       = -450.f;
    static constexpr float kLevelMinX    = -550.f;
    static constexpr float kLevelMaxX    =  1100.f;
    static constexpr float kLevelCenterX = (kLevelMinX + kLevelMaxX) * 0.5f;
    static constexpr float kLevelWidth   = kLevelMaxX - kLevelMinX;

    // Player constants
    static constexpr float     kPlayerW       = 32.f;
    static constexpr float     kPlayerH       = 48.f;
    static constexpr float     kMoveSpeed     = 280.f;
    static constexpr float     kJumpImpulse   = 360.f;   // max height ~130px at gravity -500
    static constexpr float     kJumpBufferTime = 0.15f;  // seconds a jump input is remembered
    static constexpr float     kCoyoteTime     = 0.10f;  // seconds grace after leaving ground
    static constexpr glm::vec2 kSpawnPos       = {-480.f, -220.f};
};

// ---------------------------------------------------------------------------
// Application
// ---------------------------------------------------------------------------
class ForgeApp final : public forge::Application {
public:
    ForgeApp()
        : forge::Application({"forge2D — Platformer Demo", 1280, 720, false})
        , m_camera(1280.f, 720.f)
    {
        forge::Renderer2D::init();
        forge::SceneManager::setScene(std::make_unique<PlatformerScene>());
    }

    ~ForgeApp() { forge::Renderer2D::shutdown(); }

protected:
    void onUpdate(float dt) override
    {
        forge::SceneManager::onUpdate(dt);

        // Retrieve the active scene and sync camera
        if (auto* raw = dynamic_cast<PlatformerScene*>(forge::SceneManager::getScene()))
            m_camera.setPosition(raw->getCameraPos());
    }

    void onRender() override
    {
        forge::Renderer2D::resetStats();
        forge::Renderer2D::beginScene(m_camera.getViewProjection());
        forge::SceneManager::onRender();
        forge::Renderer2D::endScene();
    }

private:
    forge::Camera2D m_camera;
};

// ---------------------------------------------------------------------------
int main()
{
    forge::Log::init(forge::LogLevel::Info);
    FG_INFO("forge2D Platformer Demo");
    FG_INFO("Controls: A/D or Left/Right = move | Space/Up = jump | R = restart");

    try {
        ForgeApp app;
        app.run();
    } catch (const std::exception& e) {
        FG_FATAL("Unhandled exception: %s", e.what());
    }
}
