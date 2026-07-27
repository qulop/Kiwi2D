#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <math/Vec4.hpp>
#include <math/Vec3.hpp>
#include <math/CommonFunc.hpp>
#include <math/SIMD.hpp>
#include <math/Angle.hpp>


namespace Kiwi {
    class Mat4 {
    public:
        using value_type = f32;
        using ValueType = f32;

        using ColumnType = Vec4;
        using column_type = Vec4;

    public:
        static constexpr size_t Size() noexcept {
            return 4;
        }

        static constexpr Mat4 Identity() noexcept {
            return Mat4(1.f);
        }

        static constexpr Mat4 Translate(const Vec3& v) noexcept {
            return glm::translate(Identity().ToGlmMat4(), v.ToGlmVec3());
        }

        static constexpr Mat4 Scale(const Vec3& v) noexcept {
            return glm::scale(Identity().m_matrix, v.ToGlmVec3());
        }

        static constexpr Mat4 Rotate(const Radians& radAngle, const Vec3& axis) noexcept {
            return glm::rotate(Identity().m_matrix,  *radAngle, axis.ToGlmVec3());
        }

        static constexpr Mat4 LookAt(const Vec3& pos, const Vec3& target, const Vec3& up) noexcept {
            return glm::lookAt(
                pos.ToGlmVec3(),
                target.ToGlmVec3(),
                up.ToGlmVec3()
            );
        }

    public:
        constexpr Mat4() = default;

        constexpr explicit Mat4(f32 scalar) :
            m_matrix(scalar)
        {}

        constexpr Mat4(const glm::mat4& mat) :
            m_matrix(mat)
        {}

        constexpr Mat4(glm::mat4&& mat) noexcept :
            m_matrix(std::move(mat))
        {}

        constexpr Mat4(const ColumnType& c0, const ColumnType& c1, const ColumnType& c2, const ColumnType& c3) :
            m_matrix(
                c0.ToGlmVec4(),
                c1.ToGlmVec4(),
                c2.ToGlmVec4(),
                c3.ToGlmVec4()
            )
        {}

        constexpr Mat4(
            f32 v00, f32 v01, f32 v02, f32 v03,
            f32 v10, f32 v11, f32 v12, f32 v13,
            f32 v20, f32 v21, f32 v22, f32 v23,
            f32 v30, f32 v31, f32 v32, f32 v33
        ) :
            Mat4(
                ColumnType{ v00, v01, v02, v03 },
                ColumnType{ v10, v11, v12, v13 },
                ColumnType{ v20, v21, v22, v23 },
                ColumnType{ v30, v31, v32, v33 }
            )
        {}


        constexpr Mat4(const Mat4& other) = default;

    public:
        constexpr const glm::vec4& operator[](const u32 column) const {
            return m_matrix[static_cast<glm::mat4::length_type>(column)];
        }

        constexpr glm::vec4& operator[](const u32 column) {
            return m_matrix[static_cast<glm::mat4::length_type>(column)];
        }

        constexpr f32& operator[](const u32 column, const u32 row) {
            return m_matrix[static_cast<glm::mat4::length_type>(column)][static_cast<glm::mat4::length_type>(row)];
        }

        KIWI_NODISCARD constexpr Mat4 operator*(const Mat4& b) const {
            using MySplatX = SplatX<4, f32, GetGlobalVectorizationMode()>;
            using MySplatY = SplatY<4, f32, GetGlobalVectorizationMode()>;
            using MySplatZ = SplatZ<4, f32, GetGlobalVectorizationMode()>;
            using MySplatW = SplatW<4, f32, GetGlobalVectorizationMode()>;


            ColumnType aRow0 = m_matrix[0];
            ColumnType aRow1 = m_matrix[1];
            ColumnType aRow2 = m_matrix[2];
            ColumnType aRow3 = m_matrix[3];

            ColumnType bRow0 = b[0];
            ColumnType bRow1 = b[1];
            ColumnType bRow2 = b[2];
            ColumnType bRow3 = b[3];

            ColumnType resRow0 = (MySplatX::Call(aRow0) * bRow0) + (MySplatY::Call(aRow0) * bRow1) + (MySplatZ::Call(aRow0) * bRow2) + (MySplatW::Call(aRow0) * bRow3);
            ColumnType resRow1 = (MySplatX::Call(aRow1) * bRow0) + (MySplatY::Call(aRow1) * bRow1) + (MySplatZ::Call(aRow1) * bRow2) + (MySplatW::Call(aRow1) * bRow3);
            ColumnType resRow2 = (MySplatX::Call(aRow2) * bRow0) + (MySplatY::Call(aRow2) * bRow1) + (MySplatZ::Call(aRow2) * bRow2) + (MySplatW::Call(aRow2) * bRow3);
            ColumnType resRow3 = (MySplatX::Call(aRow3) * bRow0) + (MySplatY::Call(aRow3) * bRow1) + (MySplatZ::Call(aRow3) * bRow2) + (MySplatW::Call(aRow3) * bRow3);

            return Mat4(resRow0, resRow1, resRow2, resRow3);
        }

        constexpr Mat4& operator*=(const Mat4& b) {
            return (*this = *this * b);
        }

        constexpr bool operator==(const glm::mat4& other) const noexcept {
            return m_matrix == other;
        }

    public:
        KIWI_NODISCARD constexpr glm::mat4 ToGlmMat4() const noexcept {
            return m_matrix;
        }

        KIWI_NODISCARD constexpr f32 GetDeterminant() const noexcept {
            return glm::determinant(m_matrix);
        }

        KIWI_NODISCARD constexpr Mat4& TranslateSelf(const Vec3& v) noexcept {
            *this = Translate(v);
            return *this;
        }

        KIWI_NODISCARD constexpr Mat4& ScaleSelf(const Vec3& v) noexcept {
            *this = Scale(v);
            return *this;
        }

        KIWI_NODISCARD constexpr Mat4& RotateSelf(const Radians& radAngle, const Vec3& axis) noexcept {
            *this = Rotate(radAngle, axis);
            return *this;
        }

    private:
        glm::mat4 m_matrix;
    };
}
