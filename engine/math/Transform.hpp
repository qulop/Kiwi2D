#pragma once

#include <math/Vec3.hpp>
#include <math/Quaternion.hpp>
#include <math/Mat4.hpp>
#include <math/Angle.hpp>

namespace Kiwi {
    class Transform {
    public:
        Transform() = default;
        Transform(const Vec3& pos, const Vec3& scale, const Quaternion& rot);

        Transform& SetPosition(const Vec3& pos);
        Transform& SetScale(const Vec3& scale);
        Transform& SetRotation(const Quaternion& rotation);

        Transform& Translate(const Vec3& offset);
        Transform& Scale(const Vec3& scale);
        Transform& RotateInLocal(const Radians& radAngle, const Vec3& axis);
        Transform& RotateInWorld(const Radians& radAngle, const Vec3& axis);

        KIWI_NODISCARD const Vec3& GetPosition() const;
        KIWI_NODISCARD const Vec3& GetScale() const;
        KIWI_NODISCARD const Quaternion& GetRotation() const;

        KIWI_NODISCARD Vec3 GetForward() const;
        KIWI_NODISCARD Vec3 GetUp() const;
        KIWI_NODISCARD Vec3 GetRight() const;

        KIWI_NODISCARD Mat4 GetViewMatrix() const;
        KIWI_NODISCARD const Mat4& GetTransformMatrix() const;

    private:
        Vec3 m_position = Vec3(0.f);
        Vec3 m_scale = Vec3(1.f);
        Quaternion m_rotation = Quaternion::Identity();

        mutable Mat4 m_transformMatrix = Mat4::Identity();
        mutable bool m_needRecalculate = true;
    };
}