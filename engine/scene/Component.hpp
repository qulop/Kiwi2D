#pragma once

#include <core/Object.hpp>


namespace Kiwi {
    class Entity;


    class Component : public AObject {
        KIWI_CREATE_OBJECT(Component, AObject);

    public:
        ~Component() override = default;

        virtual void OnInit() {}
        virtual void OnUpdate(f32 deltaTime) {}
        virtual void OnDestroy() {}

        KIWI_NODISCARD Entity* GetOwner() const { return m_owner; }

    private:
        friend class Entity;
        Entity* m_owner = nullptr;
    };
}
