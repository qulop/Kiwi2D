#include "Scene.hpp"

#include <algorithm>


namespace Kiwi {
    Entity* Scene::CreateEntity(const std::string& name) {
        return CreateEntityWithUUID(UUID::Generate(), name);
    }

    Entity* Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
        std::shared_ptr<Entity> entity = std::make_shared<Entity>(uuid, name, this);
        Entity* rawEntity = entity.get();

        m_entities.push_back(std::move(entity));

        return rawEntity;
    }

    void Scene::DestroyEntity(Entity* entity) {
        if (entity == nullptr) {
            return;
        }

        if (std::ranges::find(m_entitiesToDestroy, entity) == m_entitiesToDestroy.end()) {
            m_entitiesToDestroy.push_back(entity);
        }
    }

    void Scene::OnUpdate(f32 deltaTime) {
        for (const std::shared_ptr<Entity>& entity : m_entities) {
            entity->OnUpdate(deltaTime);
        }

        FlushDestructionQueue();
    }

    Entity* Scene::FindEntityByUUID(UUID uuid) const {
        for (const std::shared_ptr<Entity>& entity : m_entities) {
            if (entity->GetUUID() == uuid) {
                return entity.get();
            }
        }

        return nullptr;
    }

    void Scene::FlushDestructionQueue() {
        if (m_entitiesToDestroy.empty()) {
            return;
        }

        for (Entity* entity : m_entitiesToDestroy) {
            std::erase_if(m_entities, [entity](const std::shared_ptr<Entity>& candidate) {
                return candidate.get() == entity;
            });
        }

        m_entitiesToDestroy.clear();
    }
}
