#include "Entity.h"

namespace forge {

Entity::Entity(std::string name) : name(std::move(name)) {}

void Entity::onUpdate(float dt) {
    if (!active) return;
    for (auto& c : m_components) c->onUpdate(dt);
}

void Entity::onRender() {
    if (!active) return;
    for (auto& c : m_components) c->onRender();
}

} // namespace forge
