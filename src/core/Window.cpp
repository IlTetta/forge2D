#include "Window.h"
#include <glad/glad.h>    // must precede glfw3.h
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace forge {

static void onFramebufferResize(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

Window::Window(const WindowConfig& config)
    : m_width(config.width), m_height(config.height)
{
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = config.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_handle = glfwCreateWindow(m_width, m_height, config.title.c_str(), monitor, nullptr);
    if (!m_handle) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }

    glfwMakeContextCurrent(m_handle);
    glfwSetFramebufferSizeCallback(m_handle, onFramebufferResize);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("gladLoadGLLoader failed");
}

Window::~Window() {
    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_handle);
}

void Window::pollEvents() {
    glfwPollEvents();
}

} // namespace forge
