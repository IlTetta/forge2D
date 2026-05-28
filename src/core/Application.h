#pragma once
#include "Window.h"

namespace forge {

class Application {
public:
    explicit Application(const WindowConfig& config = {});
    virtual ~Application() = default;

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    void run();

protected:
    virtual void onUpdate(float dt) {}
    virtual void onRender()        {}

    Window m_window;
};

} // namespace forge
