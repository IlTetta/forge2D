#pragma once
#include <memory>

namespace forge {

class Scene;

/**
 * @brief Global manager for the active scene with deferred transition support.
 *
 * Only one scene is active at any time. Calling setScene() does not switch
 * immediately — the transition is deferred to the beginning of the next
 * onUpdate() call so that no mid-frame state is corrupted.
 *
 * @code
 * // In Application constructor:
 * forge::SceneManager::setScene(std::make_unique<MainMenuScene>());
 *
 * // Transition from within a scene:
 * forge::SceneManager::setScene(std::make_unique<GameScene>());
 * @endcode
 */
class SceneManager {
public:
    /**
     * @brief Queues a scene transition.
     *
     * The current scene's onUnload() and the new scene's onLoad() are called
     * at the start of the next onUpdate().
     *
     * @param scene The scene to transition to (ownership is transferred).
     */
    static void setScene(std::unique_ptr<Scene> scene);

    /** @brief Returns a raw pointer to the currently active scene, or nullptr. */
    static Scene* getScene();

    /** @brief Applies any pending transition, then ticks the active scene. */
    static void onUpdate(float dt);

    /** @brief Forwards the render call to the active scene. */
    static void onRender();
};

} // namespace forge
