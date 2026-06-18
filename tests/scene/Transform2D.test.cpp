#include <gtest/gtest.h>

#include <scene/Entity.hpp>
#include <scene/components/Transform2D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace Kiwi;


static constexpr float kEps = 1e-5f;


TEST(Transform2D_Test, DefaultPositionIsZero) {
    Transform2D t;
    EXPECT_FLOAT_EQ(t.position.x, 0.0f);
    EXPECT_FLOAT_EQ(t.position.y, 0.0f);
}

TEST(Transform2D_Test, DefaultRotationIsZero) {
    Transform2D t;
    EXPECT_FLOAT_EQ(t.rotation, 0.0f);
}

TEST(Transform2D_Test, DefaultScaleIsOne) {
    Transform2D t;
    EXPECT_FLOAT_EQ(t.scale.x, 1.0f);
    EXPECT_FLOAT_EQ(t.scale.y, 1.0f);
}

TEST(Transform2D_Test, DefaultModelMatrixIsIdentity) {
    Transform2D t;
    const glm::mat4 m = t.GetModelMatrix();
    const glm::mat4 identity(1.0f);
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            EXPECT_NEAR(m[col][row], identity[col][row], kEps);
        }
    }
}

TEST(Transform2D_Test, TranslationReflectedInModelMatrix) {
    Transform2D t;
    t.position = { 3.0f, 7.0f };
    const glm::mat4 m = t.GetModelMatrix();
    // Translation column is column 3
    EXPECT_NEAR(m[3][0], 3.0f, kEps);
    EXPECT_NEAR(m[3][1], 7.0f, kEps);
    EXPECT_NEAR(m[3][2], 0.0f, kEps);
}

TEST(Transform2D_Test, ScaleReflectedInModelMatrix) {
    Transform2D t;
    t.scale = { 2.0f, 4.0f };
    const glm::mat4 m = t.GetModelMatrix();
    EXPECT_NEAR(m[0][0], 2.0f, kEps);
    EXPECT_NEAR(m[1][1], 4.0f, kEps);
}

TEST(Transform2D_Test, AddedToEntityViaComponent) {
    Entity e(UUID::Generate(), "Sprite", nullptr);
    Transform2D* t = e.AddComponent<Transform2D>();
    ASSERT_NE(t, nullptr);
    t->position = { 5.0f, 10.0f };
    EXPECT_FLOAT_EQ(e.GetComponent<Transform2D>()->position.x, 5.0f);
}
