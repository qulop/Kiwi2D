#include <gtest/gtest.h>

#include <scene/Scene.hpp>
#include <scene/Entity.hpp>
#include <scene/PhysicsWorld2D.hpp>
#include <scene/components/Transform2D.hpp>
#include <scene/components/RigidBody2D.hpp>


using namespace Kiwi;


static constexpr f32 kEps = 1e-4f;


TEST(RigidBody2D_Test, DefaultsAreDynamicAtRest) {
    RigidBody2D body;
    EXPECT_EQ(body.type, EBodyType::Dynamic);
    EXPECT_FLOAT_EQ(body.velocity.x, 0.0f);
    EXPECT_FLOAT_EQ(body.velocity.y, 0.0f);
    EXPECT_FLOAT_EQ(body.gravityScale, 1.0f);
    EXPECT_FLOAT_EQ(body.GetMass(), 1.0f);
    EXPECT_FLOAT_EQ(body.GetInverseMass(), 1.0f);
}

TEST(RigidBody2D_Test, SetMassUpdatesInverseMass) {
    RigidBody2D body;
    body.SetMass(4.0f);
    EXPECT_FLOAT_EQ(body.GetMass(), 4.0f);
    EXPECT_FLOAT_EQ(body.GetInverseMass(), 0.25f);
}

TEST(RigidBody2D_Test, NonPositiveMassYieldsZeroInverse) {
    RigidBody2D body;
    body.SetMass(0.0f);
    EXPECT_FLOAT_EQ(body.GetInverseMass(), 0.0f);
}

TEST(RigidBody2D_Test, ImpulseScalesWithInverseMass) {
    RigidBody2D body;
    body.SetMass(2.0f);
    body.ApplyImpulse(Vec2{ 10.0f, 0.0f });
    EXPECT_NEAR(body.velocity.x, 5.0f, kEps);
}

TEST(RigidBody2D_Test, GravityAcceleratesDynamicBody) {
    Scene scene;
    scene.GetPhysicsWorld().SetGravity(Vec2{ 0.0f, -10.0f });

    Entity* e = scene.CreateEntity("Falling");
    e->AddComponent<Transform2D>();
    e->AddComponent<RigidBody2D>();

    scene.OnUpdate(1.0f);

    auto* body = e->GetComponent<RigidBody2D>();
    auto* transform = e->GetComponent<Transform2D>();

    // Semi-implicit Euler: v = g*dt, then position += v*dt.
    EXPECT_NEAR(body->velocity.y, -10.0f, kEps);
    EXPECT_NEAR(transform->position.y, -10.0f, kEps);
}

TEST(RigidBody2D_Test, StaticBodyDoesNotMove) {
    Scene scene;
    scene.GetPhysicsWorld().SetGravity(Vec2{ 0.0f, -10.0f });

    Entity* e = scene.CreateEntity("Ground");
    e->AddComponent<Transform2D>();
    auto* body = e->AddComponent<RigidBody2D>();
    body->type = EBodyType::Static;

    scene.OnUpdate(1.0f);

    EXPECT_NEAR(e->GetComponent<Transform2D>()->position.y, 0.0f, kEps);
    EXPECT_NEAR(body->velocity.y, 0.0f, kEps);
}

TEST(RigidBody2D_Test, KinematicBodyIgnoresGravityButMoves) {
    Scene scene;
    scene.GetPhysicsWorld().SetGravity(Vec2{ 0.0f, -10.0f });

    Entity* e = scene.CreateEntity("Platform");
    e->AddComponent<Transform2D>();
    auto* body = e->AddComponent<RigidBody2D>();
    body->type = EBodyType::Kinematic;
    body->velocity = Vec2{ 3.0f, 0.0f };

    scene.OnUpdate(1.0f);

    auto* transform = e->GetComponent<Transform2D>();
    EXPECT_NEAR(transform->position.x, 3.0f, kEps);
    EXPECT_NEAR(transform->position.y, 0.0f, kEps); // unaffected by gravity
    EXPECT_NEAR(body->velocity.x, 3.0f, kEps);
}

TEST(RigidBody2D_Test, FixedRotationKeepsRotationZero) {
    Scene scene;
    scene.GetPhysicsWorld().SetGravity(Vec2{ 0.0f, 0.0f });

    Entity* e = scene.CreateEntity("Spinner");
    e->AddComponent<Transform2D>();
    auto* body = e->AddComponent<RigidBody2D>();
    body->fixedRotation = true;
    body->angularVelocity = 5.0f;

    scene.OnUpdate(1.0f);

    EXPECT_NEAR(e->GetComponent<Transform2D>()->rotation, 0.0f, kEps);
}

TEST(RigidBody2D_Test, AppliedForceIsConsumedEachStep) {
    Scene scene;
    scene.GetPhysicsWorld().SetGravity(Vec2{ 0.0f, 0.0f });

    Entity* e = scene.CreateEntity("Pushed");
    e->AddComponent<Transform2D>();
    auto* body = e->AddComponent<RigidBody2D>();
    body->SetMass(1.0f);
    body->ApplyForce(Vec2{ 4.0f, 0.0f });

    scene.OnUpdate(1.0f);
    EXPECT_NEAR(body->velocity.x, 4.0f, kEps);

    // Force was cleared after the step, so velocity stays constant next frame.
    scene.OnUpdate(1.0f);
    EXPECT_NEAR(body->velocity.x, 4.0f, kEps);
}
