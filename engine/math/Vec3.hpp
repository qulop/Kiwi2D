#pragma once

#include <common/meta/Concepts.hpp>
#include <common/Numeric.hpp>

#include <glm/vec3.hpp>


namespace Kiwi {
    template<Concepts::Number T>
    class BasicVec3 {
    public:
        using SelfType = BasicVec3;

        using value_type = T;
        using ValueType = T;

    public:
        union {
            struct { ValueType x, y, z; };
            struct { ValueType r, g, b; };
            struct { ValueType s, t, p; };
        };

    public:
        static constexpr size_t Size() noexcept {
            return 3;
        }

        static constexpr SelfType ZeroVector() noexcept {
            return SelfType{ 0 };
        }

        static constexpr SelfType LeftVector() noexcept {
            return SelfType{ -1, 0, 0 };
        }

        static constexpr SelfType RightVector() noexcept {
            return SelfType{ 1, 0, 0 };
        }

        static constexpr SelfType UpVector() noexcept {
            return SelfType{ 0, 1, 0 };
        }

        static constexpr SelfType DownVector() noexcept {
            return SelfType{ 0, -1, 0 };
        }

        static constexpr f32 Dot(const SelfType& lhs, const SelfType& rhs) noexcept {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
        }

        static constexpr SelfType Cross(const SelfType& lhs, const SelfType& rhs) noexcept {
            return SelfType(
                lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.z * rhs.x - lhs.x * rhs.z,
                lhs.x * rhs.y - lhs.y * rhs.x
            );
        }

        static constexpr SelfType Normalize(const SelfType& vec) noexcept {
            f32 m = vec.Magnitude();
            return SelfType(vec.x / m, vec.y / m, vec.z / m);
        }

    public:
        constexpr BasicVec3() noexcept :
            x(0), y(0), z(0)
        {}

        constexpr explicit BasicVec3(ValueType scalar) noexcept :
            BasicVec3(scalar, scalar, scalar)
        {}

        constexpr BasicVec3(ValueType a, ValueType b, ValueType c) noexcept :
            x(a), y(b), z(c)
        {}

        constexpr BasicVec3(std::array<ValueType, 3> arr) noexcept :
            x(arr[0]), y(arr[1]), z(arr[2])
        {}

        constexpr BasicVec3(const glm::vec3& vec) noexcept :
            x(vec.x), y(vec.y), z(vec.z)
        {}

    public:
        constexpr BasicVec3& operator=(const BasicVec3& other) noexcept = default;
        constexpr BasicVec3& operator=(std::array<ValueType, 3> arr) noexcept {
            x = arr[0];
            y = arr[1];
            y = arr[2];

            return *this;
        }

        constexpr bool operator==(const SelfType& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }

        constexpr bool operator!=(const SelfType& other) const noexcept {
            return !(*this == other);
        }

        constexpr SelfType operator+(const SelfType& other) const noexcept {
            return SelfType(x + other.x, y + other.y, z + other.z);
        }

        constexpr SelfType operator-(const SelfType& other) const noexcept {
            return SelfType(x - other.x, y - other.y, z - other.z);
        }

        constexpr SelfType operator-() const noexcept {
            return SelfType(-x, -y, -z);
        }

        constexpr SelfType operator*(ValueType val) const noexcept {
            return SelfType(x * val, y * val, z * val);
        }

        constexpr SelfType operator*(const SelfType& other) const noexcept {
            return SelfType(x * other.x, y * other.y, z * other.z);
        }

        constexpr SelfType operator/(ValueType val) const noexcept {
            return SelfType(x / val, y / val, z / val);
        }

        constexpr SelfType operator/(const SelfType& other) const noexcept {
            return SelfType(x / other.x, y / other.y, z / other.z);
        }

        constexpr SelfType& operator+=(const SelfType& other) noexcept {
            *this = *this + other;

            return *this;
        }

        constexpr SelfType& operator+=(ValueType val) noexcept {
            *this = *this + val;

            return *this;
        }

        constexpr SelfType& operator-=(const SelfType& other) noexcept {
            *this = *this - other;

            return *this;
        }

        constexpr SelfType& operator-=(ValueType val) noexcept {
            *this = *this - val;

            return *this;
        }

        constexpr SelfType& operator*=(const SelfType& other) noexcept {
            *this = *this * other;

            return *this;
        }

        constexpr SelfType& operator*=(ValueType val) noexcept {
            *this = *this * val;

            return *this;
        }

        constexpr SelfType& operator/=(const SelfType& other) noexcept {
            *this = *this / other;

            return *this;
        }

        constexpr SelfType& operator/=(ValueType val) noexcept {
            *this = *this / val;

            return *this;
        }

        constexpr ValueType& operator[](size_t idx) noexcept {
            static_assert(idx < Size(), "Index out of range");

            switch (idx) {
            case 0:
                return x;
            case 1:
                return y;
            default:
                return z;
            }
        }

        constexpr ValueType operator[](size_t idx) const noexcept {
            static_assert(idx < Size(), "index out of range");

            switch (idx) {
            case 0:
                return x;
            case 1:
                return y;
            default:
                return z;
            }
        }

    public:
        constexpr std::array<ValueType, 3> ToArray() const noexcept {
            return { x, y, z };
        }

        constexpr glm::vec3 ToGlmVec3() const noexcept {
            return { x, y, z };
        }

        KIWI_NODISCARD constexpr f32 Magnitude() const noexcept {
            return std::sqrt(Numeric::Pow2(x) + Numeric::Pow2(y) + Numeric::Pow2(z));
        }
    };


    using Vec3 = BasicVec3<f32>;
    using I32Vec3 = BasicVec3<i32>;
    using U32Vec3 = BasicVec3<u32>;
    using BVec3 = BasicVec3<bool>;
    using DVec3 = BasicVec3<f64>;
    using Point3D = Vec3;
}
