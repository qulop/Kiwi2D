#include "RigidBody2D.hpp"


namespace Kiwi {
    void RigidBody2D::SetMass(f32 mass) {
        m_mass = mass;
        m_inverseMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    }

    void RigidBody2D::SetInertia(f32 inertia) {
        m_inertia = inertia;
        m_inverseInertia = (inertia > 0.0f) ? (1.0f / inertia) : 0.0f;
    }

    void RigidBody2D::ApplyForce(const Vec2& force) {
        m_force += force;
    }

    void RigidBody2D::ApplyTorque(f32 torque) {
        m_torque += torque;
    }

    void RigidBody2D::ApplyImpulse(const Vec2& impulse) {
        velocity += impulse * m_inverseMass;
    }

    void RigidBody2D::ApplyAngularImpulse(f32 impulse) {
        angularVelocity += impulse * m_inverseInertia;
    }

    void RigidBody2D::ClearAccumulators() {
        m_force = Vec2{ 0.0f, 0.0f };
        m_torque = 0.0f;
    }
}
