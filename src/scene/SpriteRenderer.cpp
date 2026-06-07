#include "SpriteRenderer.h"
#include "AnimationController.h"
#include "Entity.h"
#include "Transform.h"
#include "../renderer/Renderer2D.h"

namespace forge {

void SpriteRenderer::onRender() {
    auto* tr = entity->getComponent<Transform>();
    if (!tr) return;

    glm::vec2 renderSize = size * tr->scale;
    glm::vec2 bottomLeft = tr->position - renderSize * 0.5f;

    // AnimationController overrides texture and UV rect when present
    if (auto* anim = entity->getComponent<AnimationController>()) {
        if (const Texture* animTex = anim->getTexture()) {
            Renderer2D::drawSubTexture(bottomLeft, renderSize, tr->rotation,
                                      *animTex, anim->getCurrentUV(), color);
            return;
        }
    }

    if (texture)
        Renderer2D::drawQuad(bottomLeft, renderSize, tr->rotation, *texture, color);
    else
        Renderer2D::drawQuad(bottomLeft, renderSize, tr->rotation, color);
}

} // namespace forge
