#pragma once
#include <string>

struct GLFWwindow; // forward-declare to avoid pulling in glfw3.h (and its OpenGL headers) here

namespace forge {

/** @brief Configuration passed to the Window constructor. */
struct WindowConfig {
    std::string title      = "forge2d"; ///< Window title bar text.
    int         width      = 1280;      ///< Initial width in pixels.
    int         height     = 720;       ///< Initial height in pixels.
    bool        fullscreen = false;     ///< Open in fullscreen mode.
};

/**
 * @brief RAII wrapper around a GLFW window with an OpenGL 3.3 Core context.
 *
 * Initialises GLFW and creates a window + context on construction;
 * destroys both on destruction. Non-copyable.
 */
class Window {
public:
    /** @brief Creates the GLFW window and OpenGL context. Throws on failure. */
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    /** @brief Returns true when the user has requested the window to close. */
    bool shouldClose() const;

    /** @brief Swaps the front and back buffers (presents the rendered frame). */
    void swapBuffers();

    /** @brief Processes pending OS/input events (must be called each frame). */
    void pollEvents();

    int         width()  const { return m_width; }   ///< Current width in pixels.
    int         height() const { return m_height; }  ///< Current height in pixels.
    GLFWwindow* handle() const { return m_handle; }  ///< Raw GLFW window handle.

private:
    GLFWwindow* m_handle = nullptr;
    int         m_width;
    int         m_height;
};

} // namespace forge
