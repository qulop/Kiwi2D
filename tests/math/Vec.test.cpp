#include <gtest/gtest.h>

#include <math/Vec2.hpp>
#include <math/Vec3.hpp>
#include <math/Vec4.hpp>

#include <glm/glm.hpp>

namespace {
    constexpr Kiwi::Vec2 kiwiTestVec_2 = { 1.f, 2.f };
    constexpr glm::vec2 glmTestVec_2 = { 1.f, 2.f };

    constexpr Kiwi::Vec3 kiwiTestVec_3 = { 1.f, 2.f, 3.f };
    constexpr glm::vec3 glmTestVec_3 = { 1.f, 2.f, 3.f };

    constexpr Kiwi::Vec4 kiwiTestVec_4 = { 1.f, 2.f, 3.f, 4.f };
    constexpr glm::vec4 glmTestVec_4 = { 1.f, 2.f, 3.f, 4.f };
}


// Vec2
TEST(Vec2_Test, DotProduct) {
    using namespace Kiwi;

    constexpr Vec2 vecToDot = { 3.f, 4.f };

    EXPECT_EQ(Vec2::Dot(kiwiTestVec_2, vecToDot), glm::dot(glmTestVec_2, vecToDot.ToGlmVec2()));
}

TEST(Vec2_Test, Magnitude) {
    EXPECT_EQ(kiwiTestVec_2.Magnitude(), glm::length(glmTestVec_2));
}

TEST(Vec2_Test, Normalization) {
    using namespace Kiwi;

    EXPECT_EQ(Vec2::Normalize(kiwiTestVec_2), glm::normalize(glmTestVec_2));
}


// Vec3
TEST(Vec3_Test, DotProduct) {
    using namespace Kiwi;

    constexpr Vec3 vecToDot = { 4.f, 5.f, 6.f };

    EXPECT_EQ(Vec3::Dot(kiwiTestVec_3, vecToDot), glm::dot(glmTestVec_3, vecToDot.ToGlmVec3()));
}

TEST(Vec3_Test, Magnitude) {
    EXPECT_EQ(kiwiTestVec_3.Magnitude(), glm::length(glmTestVec_3));
}

TEST(Vec3_Test, Normalization) {
    using namespace Kiwi;

    EXPECT_EQ(Vec3::Normalize(kiwiTestVec_3), glm::normalize(glmTestVec_3));
}


// Vec4
TEST(Vec4_Test, DotProduct) {
    using namespace Kiwi;

    constexpr Vec4 vecToDot = { 5.f, 6.f, 7.f, 8.f };

    EXPECT_EQ(Vec4::Dot(kiwiTestVec_4, vecToDot), glm::dot(glmTestVec_4, vecToDot.ToGlmVec4()));
}

TEST(Vec4_Test, Magnitude) {
    EXPECT_EQ(kiwiTestVec_4.Magnitude(), glm::length(glmTestVec_4));
}

TEST(Vec4_Test, Normalization) {
    using namespace Kiwi;

    EXPECT_EQ(Vec4::Normalize(kiwiTestVec_4), glm::normalize(glmTestVec_4));
}