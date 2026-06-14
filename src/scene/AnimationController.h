#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace forge {

class Texture;

/**
 * @brief Describes a single animation clip — a sequence of spritesheet frames.
 *
 * Frames are linear indices into the spritesheet grid, left-to-right, top-to-bottom,
 * starting at 0.
 *
 * @code
 * forge::AnimationClip run;
 * run.frames = {2, 3, 4, 5};
 * run.fps    = 12.f;
 * run.loop   = true;
 * @endcode
 */
struct AnimationClip {
    std::vector<int> frames; ///< Ordered list of frame indices (0-based).
    float fps  = 8.f;        ///< Playback speed in frames per second.
    bool  loop = true;       ///< Whether the clip loops after the last frame.
};

/**
 * @brief Component that drives a SpriteRenderer with frame-based spritesheet animation.
 *
 * Call setSpritesheet() once to define the sprite grid, then register named clips
 * with addClip() and switch between them with play(). On each onUpdate() the
 * component advances the frame timer and updates the SpriteRenderer's UV rect.
 *
 * @code
 * auto& ac = entity.addComponent<forge::AnimationController>();
 * ac.setSpritesheet(sheet.get(), 32, 48);
 * ac.addClip("idle", {{0},       2.f, true});
 * ac.addClip("run",  {{1, 2, 3}, 12.f, true});
 * ac.play("run");
 * @endcode
 */
class AnimationController : public Component {
public:
    /**
     * @brief Defines the spritesheet texture and per-frame dimensions.
     * @param texture     The spritesheet texture (must outlive this component).
     * @param frameWidth  Width of one frame in pixels.
     * @param frameHeight Height of one frame in pixels.
     */
    void setSpritesheet(const Texture* texture, int frameWidth, int frameHeight);

    /**
     * @brief Registers a named animation clip.
     * @param name Human-readable clip identifier used by play().
     * @param clip The clip definition (frames, fps, loop flag).
     */
    void addClip(const std::string& name, AnimationClip clip);

    /**
     * @brief Switches to the named clip and resets the frame timer.
     *
     * If the clip is already playing, the call is a no-op (no restart).
     * @param name Name of a previously registered clip.
     */
    void play(const std::string& name);

    /** @brief Stops playback and freezes on the current frame. */
    void stop();

    /** @brief Returns true if a clip is currently playing. */
    bool isPlaying() const { return m_playing; }

    /** @brief Returns the spritesheet texture, or nullptr if not set. */
    const Texture* getTexture() const { return m_texture; }

    /**
     * @brief Returns the UV sub-rect {u0, v0, u1, v1} for the current frame.
     *
     * All values are in [0, 1] and ready to pass to Renderer2D::drawSubTexture().
     */
    glm::vec4 getCurrentUV() const;

    /** @brief Advances the frame timer and updates the owning SpriteRenderer. */
    void onUpdate(float dt) override;

private:
    const Texture* m_texture   = nullptr;
    int            m_frameW    = 0;
    int            m_frameH    = 0;
    int            m_cols      = 0;
    int            m_rows      = 0;

    std::unordered_map<std::string, AnimationClip> m_clips;
    const AnimationClip* m_currentClip = nullptr;
    int   m_frameIdx = 0;
    float m_elapsed  = 0.f;
    bool  m_playing  = false;
};

} // namespace forge
