#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace forge {

class Texture;

struct AnimationClip {
    std::vector<int> frames; // linear frame indices into the spritesheet grid (0-based)
    float fps  = 8.f;
    bool  loop = true;
};

class AnimationController : public Component {
public:
    // Define the spritesheet layout. frameWidth/frameHeight in pixels.
    void setSpritesheet(const Texture* texture, int frameWidth, int frameHeight);

    void addClip(const std::string& name, AnimationClip clip);

    void play(const std::string& name);
    void stop();

    bool        isPlaying()  const { return m_playing; }
    const Texture* getTexture() const { return m_texture; }

    // UV sub-rect {u0, v0, u1, v1} for the current frame, ready for drawQuad.
    glm::vec4 getCurrentUV() const;

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
