#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

namespace forge {

// Key constants — values match GLFW_KEY_*
namespace Key {
    constexpr int Space        = 32;
    constexpr int Num0=48, Num1=49, Num2=50, Num3=51, Num4=52;
    constexpr int Num5=53, Num6=54, Num7=55, Num8=56, Num9=57;
    constexpr int A=65,  B=66,  C=67,  D=68,  E=69,  F=70;
    constexpr int G=71,  H=72,  I=73,  J=74,  K=75,  L=76;
    constexpr int M=77,  N=78,  O=79,  P=80,  Q=81,  R=82;
    constexpr int S=83,  T=84,  U=85,  V=86,  W=87,  X=88;
    constexpr int Y=89,  Z=90;
    constexpr int Escape      = 256;
    constexpr int Enter       = 257;
    constexpr int Tab         = 258;
    constexpr int Backspace   = 259;
    constexpr int Right       = 262;
    constexpr int Left        = 263;
    constexpr int Down        = 264;
    constexpr int Up          = 265;
    constexpr int F1=290,  F2=291,  F3=292,  F4=293;
    constexpr int F5=294,  F6=295,  F7=296,  F8=297;
    constexpr int F9=298,  F10=299, F11=300, F12=301;
    constexpr int LeftShift    = 340;
    constexpr int LeftControl  = 341;
    constexpr int LeftAlt      = 342;
    constexpr int RightShift   = 344;
    constexpr int RightControl = 345;
    constexpr int RightAlt     = 346;
}

// Mouse button constants — values match GLFW_MOUSE_BUTTON_*
namespace Mouse {
    constexpr int Left   = 0;
    constexpr int Right  = 1;
    constexpr int Middle = 2;
}

class Input {
public:
    static void init  (GLFWwindow* window);
    static void update();  // call once per frame, after onUpdate() and before swapBuffers()

    // Keyboard
    static bool isKeyDown    (int key);  // held down (includes the first frame)
    static bool isKeyPressed (int key);  // true only on the first frame of a press
    static bool isKeyReleased(int key);  // true only on the first frame of a release

    // Mouse buttons
    static bool isMouseDown    (int button);
    static bool isMousePressed (int button);
    static bool isMouseReleased(int button);

    // Mouse position (screen pixels, origin top-left)
    static glm::vec2 getMousePos();

    // Scroll wheel — accumulated since last update(), reset each frame
    static float getScrollDelta();

private:
    static void keyCallback        (GLFWwindow*, int key, int, int action, int);
    static void mouseButtonCallback(GLFWwindow*, int button, int action, int);
    static void scrollCallback     (GLFWwindow*, double, double yoffset);
    static void cursorPosCallback  (GLFWwindow*, double x, double y);
};

} // namespace forge
