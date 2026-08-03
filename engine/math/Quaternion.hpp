#pragma once

#include <common/Definitions.hpp>

#include <math/Angle.hpp>
#include <math/Vec3.hpp>
#include <math/Mat4.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/detail/type_quat.hpp>

#define GLM_ENABLE_EXPERIMENTAL
    #include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL



namespace Kiwi {
    class Quaternion {
    public:
        KIWI_NODISCARD static constexpr Quaternion Identity() noexcept {
            return Quaternion(1.f, 0.f, 0.f, 0.f);
        }

        KIWI_NODISCARD static constexpr Quaternion FromXYZW(f32 x, f32 y, f32 z, f32 w) noexcept {
            return Quaternion(x, y, z, w);
        }

        KIWI_NODISCARD static constexpr Quaternion FromAngleAndAxis(const Radians& radAngle, const Vec3& axis) noexcept {
            return Quaternion(glm::angleAxis(
                *radAngle,
                Vec3::Normalize(axis).ToGlmVec3()
            ));
        }

        KIWI_NODISCARD static constexpr Quaternion FromEulerAngles(const Radians& radX, const Radians& radY, const Radians& radZ) noexcept {
            return Quaternion(
                glm::quat(
                    glm::vec3{ *radX, *radY, *radZ }
                )
            );
        }

        KIWI_NODISCARD static constexpr Quaternion Normalize(const Quaternion& q) noexcept {
            return Quaternion(glm::normalize(q.m_quat));
        }

    public:
        constexpr Quaternion() = default;

        constexpr explicit Quaternion(f32 x, f32 y, f32 z, f32 w) :
            m_quat(w, x, y, z)
        {}

        constexpr explicit Quaternion(const glm::quat& glmQuat) :
            m_quat(glmQuat)
        {}

        KIWI_NODISCARD constexpr Quaternion operator*(const Quaternion& other) const noexcept {
            return Quaternion(m_quat * other.m_quat);
        }

        KIWI_NODISCARD constexpr Quaternion operator*(const glm::quat& other) const noexcept {
            return Quaternion(m_quat * other);
        }

        KIWI_NODISCARD constexpr Vec3 operator*(const Vec3& v) const noexcept {
            return Vec3(m_quat * v.ToGlmVec3());
        }

        KIWI_NODISCARD constexpr Vec4 GetComponents() const noexcept {
            return Vec4(
                m_quat.x,
                m_quat.y,
                m_quat.z,
                m_quat.w
            );
        }

        KIWI_NODISCARD constexpr EulerAngles ToEulerAngles() const noexcept {
            const glm::vec3 e = glm::eulerAngles(m_quat);
            return EulerAngles {
                .pitch = Radians(e.x),
                .yaw = Radians(e.y),
                .roll = Radians(e.z)
            };
        }

        KIWI_NODISCARD constexpr Mat4 ToMat4() const noexcept {
            return Mat4(glm::toMat4(m_quat));
        }

        KIWI_NODISCARD constexpr Quaternion Inversed() const noexcept {
            return Quaternion(glm::inverse(m_quat));
        }

        KIWI_NODISCARD constexpr Vec3 RotateVector(const Vec3& v) const noexcept {
            return *this * v;
        }

    private:
        glm::quat m_quat;
    };
}
