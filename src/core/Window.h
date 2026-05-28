#pragma once
#include <string>

struct GLFWwindow; // forward-declare to avoid pulling in glfw3.h (and its OpenGL headers) here

namespace forge {

struct WindowConfig {
    std::string title      = "forge2d";
    int         width      = 1280;
    int         height     = 720;
    bool        fullscreen = false;
};

class Window {
public:
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    int          width()  const { return m_width; }
    int          height() const { return m_height; }
    GLFWwindow*  handle() const { return m_handle; }

private:
    GLFWwindow* m_handle = nullptr;
    int         m_width;
    int         m_height;
};

} // namespace forge
