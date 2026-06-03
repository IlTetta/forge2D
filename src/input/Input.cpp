#include "Input.h"
#include <GLFW/glfw3.h>
#include <array>

namespace forge {

enum class KeyState : uint8_t { Up, Pressed, Held, Released };

static constexpr int KEY_COUNT   = GLFW_KEY_LAST + 1;        // 349
static constexpr int MOUSE_COUNT = GLFW_MOUSE_BUTTON_LAST + 1; // 8

static std::array<KeyState, KEY_COUNT>   s_keys{};
static std::array<KeyState, MOUSE_COUNT> s_mouse{};
static glm::vec2 s_mousePos{0.f};
static float     s_scrollDelta = 0.f;

// --------------------------------------------------------------------------

void Input::init(GLFWwindow* window) {
    glfwSetKeyCallback        (window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback     (window, scrollCallback);
    glfwSetCursorPosCallback  (window, cursorPosCallback);
}

void Input::update() {
    for (auto& k : s_keys) {
        if (k == KeyState::Pressed)  k = KeyState::Held;
        if (k == KeyState::Released) k = KeyState::Up;
    }
    for (auto& b : s_mouse) {
        if (b == KeyState::Pressed)  b = KeyState::Held;
        if (b == KeyState::Released) b = KeyState::Up;
    }
    s_scrollDelta = 0.f;
}

// --------------------------------------------------------------------------

bool Input::isKeyDown    (int k) { return s_keys[k] == KeyState::Pressed || s_keys[k] == KeyState::Held; }
bool Input::isKeyPressed (int k) { return s_keys[k] == KeyState::Pressed; }
bool Input::isKeyReleased(int k) { return s_keys[k] == KeyState::Released; }

bool Input::isMouseDown    (int b) { return s_mouse[b] == KeyState::Pressed || s_mouse[b] == KeyState::Held; }
bool Input::isMousePressed (int b) { return s_mouse[b] == KeyState::Pressed; }
bool Input::isMouseReleased(int b) { return s_mouse[b] == KeyState::Released; }

glm::vec2 Input::getMousePos()    { return s_mousePos; }
float     Input::getScrollDelta() { return s_scrollDelta; }

// --------------------------------------------------------------------------

void Input::keyCallback(GLFWwindow*, int key, int, int action, int) {
    if (key < 0 || key >= KEY_COUNT) return;
    if (action == GLFW_PRESS)   s_keys[key] = KeyState::Pressed;
    if (action == GLFW_RELEASE) s_keys[key] = KeyState::Released;
    // GLFW_REPEAT is ignored — isKeyDown() returns true throughout the Held state
}

void Input::mouseButtonCallback(GLFWwindow*, int button, int action, int) {
    if (button < 0 || button >= MOUSE_COUNT) return;
    if (action == GLFW_PRESS)   s_mouse[button] = KeyState::Pressed;
    if (action == GLFW_RELEASE) s_mouse[button] = KeyState::Released;
}

void Input::scrollCallback(GLFWwindow*, double, double yoffset) {
    s_scrollDelta += static_cast<float>(yoffset);
}

void Input::cursorPosCallback(GLFWwindow*, double x, double y) {
    s_mousePos = { static_cast<float>(x), static_cast<float>(y) };
}

} // namespace forge
