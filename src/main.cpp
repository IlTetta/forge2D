#include "core/Application.h"
#include "core/Log.h"

class ForgeApp final : public forge::Application {
public:
    ForgeApp()
        : forge::Application({"forge2d", 1280, 720, false}) {}

protected:
    void onUpdate(float dt) override {}
    void onRender()         override {}
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
