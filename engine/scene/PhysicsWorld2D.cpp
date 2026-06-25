#include "PhysicsWorld2D.hpp"

#include <scene/Scene.hpp>
#include <scene/Entity.hpp>
#include <scene/components/Transform2D.hpp>
#include <scene/components/RigidBody2D.hpp>


namespace Kiwi {
    void PhysicsWorld2D::Step(Scene& scene, f32 deltaTime) {
        if (deltaTime <= 0.0f) {
            return;
        }

        for (const std::shared_ptr<Entity>& entity : scene.GetEntities()) {
            auto* body = entity->GetComponent<RigidBody2D>();
            auto* transform = entity->GetComponent<Transform2D>();
            if (body == nullptr || transform == nullptr || !body->IsSimulated()) {
                continue;
            }

            // Dynamic bodies accumulate acceleration from gravity and applied forces;
            // kinematic bodies coast on whatever velocity they were given.
            if (body->type == EBodyType::Dynamic) {
                const Vec2 acceleration =
                    body->m_force * body->m_inverseMass + m_gravity * body->gravityScale;
                body->velocity += acceleration * deltaTime;

                const f32 angularAcceleration = body->m_torque * body->m_inverseInertia;
                body->angularVelocity += angularAcceleration * deltaTime;
            }

            // Exponential-style damping that stays stable regardless of the step size.
            body->velocity *= 1.0f / (1.0f + body->linearDamping * deltaTime);
            body->angularVelocity *= 1.0f / (1.0f + body->angularDamping * deltaTime);

            // Semi-implicit Euler: integrate position using the freshly updated velocity.
            transform->position += body->velocity * deltaTime;
            if (!body->fixedRotation) {
                transform->rotation += body->angularVelocity * deltaTime;
            }

            body->ClearAccumulators();
        }
    }
}
