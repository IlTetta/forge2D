#include "Scene.h"
#include "Transform.h"
#include <algorithm>

namespace forge {

Scene::Scene(std::string name) : m_name(std::move(name)) {}

Entity& Scene::createEntity(const std::string& name) {
    auto& e = *m_entities.emplace_back(std::make_unique<Entity>(name));
    e.addComponent<Transform>(); // every entity starts with a Transform
    return e;
}

void Scene::destroyEntity(const std::string& name) {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
                           [&](const auto& e) { return e->name == name; });
    if (it != m_entities.end()) m_entities.erase(it);
}

Entity* Scene::findEntity(const std::string& name) {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
                           [&](const auto& e) { return e->name == name; });
    return it != m_entities.end() ? it->get() : nullptr;
}

void Scene::onUpdate(float dt) {
    for (auto& e : m_entities) e->onUpdate(dt);
}

void Scene::onRender() {
    for (auto& e : m_entities) e->onRender();
}

} // namespace forge
