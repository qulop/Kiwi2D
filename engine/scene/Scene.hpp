#pragma once

#include <core/Object.hpp>

#include <common/UUID.hpp>

#include <scene/Entity.hpp>

#include <vector>
#include <memory>
#include <string>


namespace Kiwi {
    class Scene : public AObject {
        KIWI_CREATE_OBJECT(Scene, AObject);

    public:
        Scene() = default;
        ~Scene() override = default;

        KIWI_NODISCARD Entity* CreateEntity(const std::string& name = "Entity");
        KIWI_NODISCARD Entity* CreateEntityWithUUID(UUID uuid, const std::string& name = "Entity");

        void DestroyEntity(Entity* entity);

        void OnUpdate(f32 deltaTime);

        KIWI_NODISCARD Entity* FindEntityByUUID(UUID uuid) const;

        KIWI_NODISCARD const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_entities; }

    private:
        void FlushDestructionQueue();

    private:
        std::vector<std::shared_ptr<Entity>> m_entities;
        std::vector<Entity*> m_entitiesToDestroy;
    };
}
