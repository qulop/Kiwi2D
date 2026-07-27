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

    Transform& Transform::RotateLocal(const Radians& radAngle, const Vec3& axis) {
        const Quaternion delta = Quaternion::FromAngleAndAxis(radAngle, axis);
        m_rotation = Quaternion::Normalize(
            m_rotation * delta
        );
        m_needRecalculate = true;

        return *this;
    }

    Transform& Transform::RotateWorld(const Radians& radAngle, const Vec3& axis) {
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
