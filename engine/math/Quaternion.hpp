#pragma once

#include <common/Definitions.hpp>

#include <math/Angle.hpp>
#include <math/Vec3.hpp>
#include <math/Mat4.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/detail/type_quat.hpp>



namespace Kiwi {
    class Quaternion {
    public:
        KIWI_NODISCARD static constexpr Quaternion Identity() noexcept {
            return Quaternion(1.f, 0.f, 0.f, 0.f);
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

        constexpr explicit Quaternion(f32 w, f32 x, f32 y, f32 z) :
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

        KIWI_NODISCARD constexpr Mat4 ToMat4() const noexcept {
            return Mat4(glm::toMat4(m_quat));
        }

    private:
        glm::quat m_quat;
    };
}
