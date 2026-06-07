#pragma once
#include <memory>

namespace forge {

class Scene;

class SceneManager {
public:
    // Queue a scene transition. The switch happens at the start of the next
    // onUpdate so no mid-frame state is corrupted.
    static void setScene(std::unique_ptr<Scene> scene);

    static Scene*       getScene();
    static void         onUpdate(float dt);
    static void         onRender();
};

} // namespace forge
