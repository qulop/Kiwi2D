#pragma once

#include <core/Object.hpp>

#include <math/vec/Vec2.hpp>

#include <glm/glm.hpp>


namespace Kiwi {
    class Camera2D : public AObject {
        KIWI_CREATE_OBJECT(Camera2D, AObject);

    public:
        Camera2D(f32 left, f32 right, f32 bottom, f32 top);
        ~Camera2D() override = default;

        void SetProjection(f32 left, f32 right, f32 bottom, f32 top);

        KIWI_NODISCARD const Vec2& GetPosition() const { return m_position; }
        void SetPosition(const Vec2& position) { m_position = position; RecalculateViewMatrix(); }

        KIWI_NODISCARD f32 GetRotation() const { return m_rotation; }
        void SetRotation(f32 rotation) { m_rotation = rotation; RecalculateViewMatrix(); }

        KIWI_NODISCARD const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
        KIWI_NODISCARD const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
        KIWI_NODISCARD const glm::mat4& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }

    private:
        void RecalculateViewMatrix();

    private:
        glm::mat4 m_projectionMatrix{ 1.0f };
        glm::mat4 m_viewMatrix{ 1.0f };
        glm::mat4 m_viewProjectionMatrix{ 1.0f };

        Vec2 m_position{ 0.0f, 0.0f };
        f32 m_rotation = 0.0f;
    };
}
