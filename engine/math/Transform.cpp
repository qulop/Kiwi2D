#include "Transform.hpp"

namespace Kiwi {
    Transform::Transform(const Vec3& pos, const Vec3& scale, const Quaternion& rot) :
        m_position(pos),
        m_scale(scale),
        m_rotation(rot)
    {}

    Transform& Transform::SetPosition(const Vec3& pos) {
        m_position = pos;
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::SetScale(const Vec3& scale) {
        m_scale = scale;
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::SetRotation(const Quaternion& rotation) {
        m_rotation = rotation;
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::Translate(const Vec3& offset) {
        m_position += offset;
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::Scale(const Vec3& scale) {
        m_scale *= scale;
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::RotateInLocal(const Radians& radAngle, const Vec3& axis) {
        const Quaternion delta = Quaternion::FromAngleAndAxis(radAngle, axis);
        m_rotation = Quaternion::Normalize(
            m_rotation * delta
        );
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::RotateInWorld(const Radians& radAngle, const Vec3& axis) {
        const Quaternion delta = Quaternion::FromAngleAndAxis(radAngle, axis);
        m_rotation = Quaternion::Normalize(
            delta * m_rotation
        );
        m_needRecalculate = true;

        return *this;
    }

    const Vec3& Transform::GetPosition() const {
        return m_position;
    }

    const Vec3& Transform::GetScale() const {
        return m_scale;
    }

    const Quaternion& Transform::GetRotation() const {
        return m_rotation;
    }

    Vec3 Transform::GetForward() const {
        return m_rotation.RotateVector(Vec3(0.f, 0.f, -1.f));
    }

    Vec3 Transform::GetUp() const {
        return m_rotation.RotateVector(Vec3::UpVector());
    }

    Vec3 Transform::GetRight() const {
        return m_rotation.RotateVector(Vec3::RightVector());
    }

    Mat4 Transform::GetViewMatrix() const {
        return Mat4::LookAt(
            GetPosition(),
            GetPosition() + GetForward(),
            GetUp()
        );
    }

    const Mat4& Transform::GetTransformMatrix() const {
        if (m_needRecalculate) {
            m_transformMatrix =
                Mat4::Translate(m_position)
                * m_rotation.ToMat4()
                * Mat4::Scale(m_scale);

            m_needRecalculate = false;
        }

        return m_transformMatrix;
    }
}
