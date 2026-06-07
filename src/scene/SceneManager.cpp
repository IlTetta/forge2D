#include "SceneManager.h"
#include "Scene.h"
#include "../core/Log.h"

namespace forge {

static std::unique_ptr<Scene> s_current;
static std::unique_ptr<Scene> s_pending;

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    s_pending = std::move(scene);
}

Scene* SceneManager::getScene() { return s_current.get(); }

void SceneManager::onUpdate(float dt) {
    // Apply pending transition before the update tick
    if (s_pending) {
        if (s_current) {
            s_current->onUnload();
            FG_INFO("Scene unloaded: %s", s_current->getName().c_str());
        }
        s_current = std::move(s_pending);
        s_current->onLoad();
        FG_INFO("Scene loaded: %s", s_current->getName().c_str());
    }

    if (s_current) s_current->onUpdate(dt);
}

void SceneManager::onRender() {
    if (s_current) s_current->onRender();
}

} // namespace forge
