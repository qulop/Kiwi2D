#pragma once

#include <scene/Component.hpp>

#include <math/vec/Vec2.hpp>

#include <glm/glm.hpp>


namespace Kiwi {
    class Transform2D : public Component {
        KIWI_CREATE_OBJECT(Transform2D, Component);

    public:
        Vec2 position{ 0.0f, 0.0f };
        f32 rotation = 0.0f; // In radians
        Vec2 scale{ 1.0f, 1.0f };

        KIWI_NODISCARD glm::mat4 GetModelMatrix() const;

        ~Transform2D() override = default;
    };
}
