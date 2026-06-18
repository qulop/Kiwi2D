#include "Entity.hpp"


namespace Kiwi {
    Entity::Entity(UUID id, const std::string& name, Scene* scene) :
        m_id(id),
        m_name(name),
        m_scene(scene)
    {}

    void Entity::OnUpdate(f32 deltaTime) {
        for (const std::shared_ptr<Component>& component : m_components) {
            component->OnUpdate(deltaTime);
        }
    }
}
