#include "Application.h"
#include "Log.h"
#include "../input/Input.h"
#include <glad/glad.h>    // must precede glfw3.h
#include <GLFW/glfw3.h>

namespace forge {

static constexpr double FIXED_DT = 1.0 / 60.0;

Application::Application(const WindowConfig& config)
    : m_window(config)
{
    FG_INFO("Window created  %dx%d  \"%s\"",
            config.width, config.height, config.title.c_str());
    Input::init(m_window.handle());
}

void Application::run() {
    FG_INFO("forge2d loop started");
    double previous = glfwGetTime();
    double lag      = 0.0;

    while (!m_window.shouldClose()) {
        double current = glfwGetTime();
        double elapsed = current - previous;
        previous = current;
        lag += elapsed;

        m_window.pollEvents();

        while (lag >= FIXED_DT) {
            onUpdate(static_cast<float>(FIXED_DT));
            lag -= FIXED_DT;
        }
        Input::update(); // tick key states, reset scroll delta

        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        onRender();

        m_window.swapBuffers();
    }

    FG_INFO("forge2d loop ended");
}

} // namespace forge
