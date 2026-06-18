#include <gtest/gtest.h>

#include <scene/Entity.hpp>
#include <scene/Component.hpp>
#include <scene/components/Transform2D.hpp>
#include <common/UUID.hpp>


using namespace Kiwi;


// Minimal concrete component for testing lifecycle callbacks
class TestComponent : public Component {
    KIWI_CREATE_OBJECT(TestComponent, Component);
public:
    bool initCalled    = false;
    bool destroyCalled = false;

    void OnInit()    override { initCalled    = true; }
    void OnDestroy() override { destroyCalled = true; }
};


static Entity MakeEntity(const std::string& name = "TestEntity") {
    return Entity(UUID::Generate(), name, nullptr);
}


TEST(Entity_Test, NameIsStoredCorrectly) {
    Entity e = MakeEntity("Player");
    EXPECT_EQ(e.GetName(), "Player");
}

TEST(Entity_Test, UUIDIsNotNil) {
    Entity e = MakeEntity();
    EXPECT_NE(e.GetUUID(), UUID{});
}

TEST(Entity_Test, AddComponentReturnsNonNull) {
    Entity e = MakeEntity();
    Transform2D* t = e.AddComponent<Transform2D>();
    EXPECT_NE(t, nullptr);
}

TEST(Entity_Test, GetComponentFindsAddedComponent) {
    Entity e = MakeEntity();
    Transform2D* added = e.AddComponent<Transform2D>();
    Transform2D* found = e.GetComponent<Transform2D>();
    EXPECT_EQ(added, found);
}

TEST(Entity_Test, HasComponentReturnsTrueAfterAdd) {
    Entity e = MakeEntity();
    EXPECT_FALSE(e.HasComponent<Transform2D>());
    e.AddComponent<Transform2D>();
    EXPECT_TRUE(e.HasComponent<Transform2D>());
}

TEST(Entity_Test, AddDuplicateComponentReturnsExisting) {
    Entity e = MakeEntity();
    Transform2D* first  = e.AddComponent<Transform2D>();
    Transform2D* second = e.AddComponent<Transform2D>();
    EXPECT_EQ(first, second);
}

TEST(Entity_Test, RemoveComponentMakesHasComponentFalse) {
    Entity e = MakeEntity();
    e.AddComponent<Transform2D>();
    e.RemoveComponent<Transform2D>();
    EXPECT_FALSE(e.HasComponent<Transform2D>());
}

TEST(Entity_Test, OnInitCalledAfterAddComponent) {
    Entity e = MakeEntity();
    TestComponent* c = e.AddComponent<TestComponent>();
    EXPECT_TRUE(c->initCalled);
}

TEST(Entity_Test, OnDestroyCalledOnRemoveComponent) {
    Entity e = MakeEntity();
    e.AddComponent<TestComponent>();
    TestComponent* c = e.GetComponent<TestComponent>();
    EXPECT_FALSE(c->destroyCalled);
    e.RemoveComponent<TestComponent>();
    EXPECT_TRUE(c->destroyCalled);
}

TEST(Entity_Test, ComponentOwnerIsSetToEntity) {
    Entity e = MakeEntity();
    TestComponent* c = e.AddComponent<TestComponent>();
    EXPECT_EQ(c->GetOwner(), &e);
}

TEST(Entity_Test, SetNameUpdatesName) {
    Entity e = MakeEntity("OldName");
    e.SetName("NewName");
    EXPECT_EQ(e.GetName(), "NewName");
}
