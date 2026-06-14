#include "AnimationController.h"
#include "../renderer/Texture.h"
#include "../core/Log.h"

namespace forge {

void AnimationController::setSpritesheet(const Texture* texture,
                                          int frameWidth, int frameHeight)
{
    m_texture = texture;
    m_frameW  = frameWidth;
    m_frameH  = frameHeight;
    m_cols    = texture ? texture->width()  / frameWidth  : 0;
    m_rows    = texture ? texture->height() / frameHeight : 0;
}

void AnimationController::addClip(const std::string& name, AnimationClip clip) {
    m_clips[name] = std::move(clip);
}

void AnimationController::play(const std::string& name) {
    auto it = m_clips.find(name);
    if (it == m_clips.end()) {
        FG_WARN("AnimationController: clip '%s' not found", name.c_str());
        return;
    }
    m_currentClip = &it->second;
    m_frameIdx    = 0;
    m_elapsed     = 0.f;
    m_playing     = true;
}

void AnimationController::stop() {
    m_playing = false;
}

glm::vec4 AnimationController::getCurrentUV() const {
    if (!m_currentClip || m_currentClip->frames.empty() || !m_texture || m_cols == 0)
        return {0.f, 0.f, 1.f, 1.f};

    int linear = m_currentClip->frames[m_frameIdx];
    int col    = linear % m_cols;
    int row    = linear / m_cols;

    // UV formula consistent with stbi_set_flip_vertically_on_load(true):
    //   row 0 of the image (top visually) maps to UV v in [1-fh, 1].
    //   Also works for single-row spritesheets regardless of flip.
    float fw = 1.f / (float)m_cols;
    float fh = 1.f / (float)m_rows;
    float u0 =  col      * fw;
    float v0 = 1.f - (row + 1) * fh;
    float u1 = (col + 1) * fw;
    float v1 = 1.f -  row      * fh;

    return {u0, v0, u1, v1};
}

void AnimationController::onUpdate(float dt) {
    if (!m_playing || !m_currentClip || m_currentClip->frames.empty()) return;

    m_elapsed += dt;
    const float frameDuration = 1.f / m_currentClip->fps;

    while (m_elapsed >= frameDuration) {
        m_elapsed -= frameDuration;
        ++m_frameIdx;

        if (m_frameIdx >= (int)m_currentClip->frames.size()) {
            if (m_currentClip->loop) {
                m_frameIdx = 0;
            } else {
                m_frameIdx = (int)m_currentClip->frames.size() - 1;
                m_playing  = false;
                break;
            }
        }
    }
}

} // namespace forge
