#pragma once
#include "Window.h"

namespace forge {

/**
 * @brief Base class for a forge2D application.
 *
 * Subclass Application, override onUpdate() and onRender(), then call run()
 * from main(). The game loop runs at a fixed 60 Hz update rate with an
 * uncapped render step.
 *
 * @code
 * class MyApp : public forge::Application {
 * protected:
 *     void onUpdate(float dt) override { ... }
 *     void onRender()         override { ... }
 * };
 *
 * int main() {
 *     MyApp app;
 *     app.run();
 * }
 * @endcode
 */
class Application {
public:
    /** @brief Constructs the application and opens the GLFW window. */
    explicit Application(const WindowConfig& config = {});
    virtual ~Application() = default;

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    /** @brief Enters the game loop. Blocks until the window is closed. */
    void run();

protected:
    /**
     * @brief Called once per fixed timestep (default 1/60 s).
     * @param dt Delta time in seconds (fixed at ~0.01667 s).
     */
    virtual void onUpdate(float dt) {}

    /** @brief Called every frame, after all onUpdate() calls. */
    virtual void onRender() {}

    Window m_window; ///< The GLFW-backed window managed by this application.
};

} // namespace forge
