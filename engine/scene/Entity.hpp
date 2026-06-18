#pragma once

#include <core/Object.hpp>

#include <common/UUID.hpp>
#include <common/meta/Concepts.hpp>

#include <scene/Component.hpp>

#include <vector>
#include <memory>
#include <string>


namespace Kiwi {
    class Scene;


    class Entity : public AObject {
        KIWI_CREATE_OBJECT(Entity, AObject);

    public:
        Entity(UUID id, const std::string& name, Scene* scene);
        ~Entity() override = default;

        template<Concepts::DerivedFrom<Component> T, typename... Args>
        T* AddComponent(Args&&... args) {
            if (T* existing = GetComponent<T>()) {
                return existing;
            }

            std::shared_ptr<T> component = std::make_shared<T>(std::forward<Args>(args)...);
            component->m_owner = this;
            component->OnInit();

            T* rawComponent = component.get();
            m_components.push_back(std::move(component));

            return rawComponent;
        }

        template<Concepts::DerivedFrom<Component> T>
        KIWI_NODISCARD T* GetComponent() const {
            for (const std::shared_ptr<Component>& component : m_components) {
                if (component->GetType() == T::GetStaticType()) {
                    return static_cast<T*>(component.get());
                }
            }

            return nullptr;
        }

        template<Concepts::DerivedFrom<Component> T>
        KIWI_NODISCARD bool HasComponent() const {
            return GetComponent<T>() != nullptr;
        }

        template<Concepts::DerivedFrom<Component> T>
        void RemoveComponent() {
            for (auto it = m_components.begin(); it != m_components.end(); ++it) {
                if ((*it)->GetType() == T::GetStaticType()) {
                    (*it)->OnDestroy();
                    m_components.erase(it);
                    return;
                }
            }
        }

        void OnUpdate(f32 deltaTime);

        KIWI_NODISCARD UUID GetUUID() const { return m_id; }
        KIWI_NODISCARD const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        KIWI_NODISCARD Scene* GetScene() const { return m_scene; }

    private:
        UUID m_id;
        std::string m_name;
        Scene* m_scene = nullptr;
        std::vector<std::shared_ptr<Component>> m_components;
    };
}
