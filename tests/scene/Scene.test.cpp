#include <gtest/gtest.h>

#include <scene/Scene.hpp>
#include <scene/Entity.hpp>
#include <scene/components/Transform2D.hpp>
#include <common/UUID.hpp>


using namespace Kiwi;


TEST(Scene_Test, CreateEntityReturnsNonNull) {
    Scene scene;
    Entity* e = scene.CreateEntity("Hero");
    EXPECT_NE(e, nullptr);
}

TEST(Scene_Test, CreateEntitySetsName) {
    Scene scene;
    Entity* e = scene.CreateEntity("Enemy");
    EXPECT_EQ(e->GetName(), "Enemy");
}

TEST(Scene_Test, EntityCountIncreasesAfterCreate) {
    Scene scene;
    EXPECT_EQ(scene.GetEntities().size(), 0u);
    scene.CreateEntity("A");
    scene.CreateEntity("B");
    EXPECT_EQ(scene.GetEntities().size(), 2u);
}

TEST(Scene_Test, FindEntityByUUIDReturnsCorrectEntity) {
    Scene scene;
    Entity* e = scene.CreateEntity("Bullet");
    const UUID id = e->GetUUID();
    Entity* found = scene.FindEntityByUUID(id);
    EXPECT_EQ(found, e);
}

TEST(Scene_Test, FindEntityByUUIDReturnNullForUnknown) {
    Scene scene;
    scene.CreateEntity("X");
    Entity* found = scene.FindEntityByUUID(UUID::Generate());
    EXPECT_EQ(found, nullptr);
}

TEST(Scene_Test, CreateEntityWithExplicitUUID) {
    Scene scene;
    const UUID id = UUID::Generate();
    Entity* e = scene.CreateEntityWithUUID(id, "Custom");
    EXPECT_EQ(e->GetUUID(), id);
}

TEST(Scene_Test, DestroyEntityRemovesAfterOnUpdate) {
    Scene scene;
    Entity* e = scene.CreateEntity("Temp");
    EXPECT_EQ(scene.GetEntities().size(), 1u);
    scene.DestroyEntity(e);
    // Destruction is deferred — entity still alive before the next update
    EXPECT_EQ(scene.GetEntities().size(), 1u);
    scene.OnUpdate(0.0f);
    EXPECT_EQ(scene.GetEntities().size(), 0u);
}

TEST(Scene_Test, MultipleEntitiesHaveUniqueUUIDs) {
    Scene scene;
    Entity* a = scene.CreateEntity("A");
    Entity* b = scene.CreateEntity("B");
    EXPECT_NE(a->GetUUID(), b->GetUUID());
}
