#pragma once

#include <scene/Component.hpp>

#include <math/vec/Vec2.hpp>

#include <common/Definitions.hpp>


namespace Kiwi {
    // How the body reacts to forces and integration:
    //  - Static    : never moves; treated as having infinite mass.
    //  - Kinematic : moved only through its velocity, ignores forces and gravity.
    //  - Dynamic   : fully simulated (gravity, forces, impulses).
    enum class EBodyType : u8 {
        Static,
        Kinematic,
        Dynamic
    };


    // Holds the dynamic state of a 2D body. The actual integration is performed by
    // PhysicsWorld2D, which reads/writes this component together with the owner's
    // Transform2D. Mass and inertia are kept in sync with their inverses so the
    // integrator can multiply instead of divide.
    class RigidBody2D : public Component {
        KIWI_CREATE_OBJECT(RigidBody2D, Component);

    public:
        EBodyType type = EBodyType::Dynamic;

        Vec2 velocity{ 0.0f, 0.0f };    // world units / second
        f32 angularVelocity = 0.0f;     // radians / second

        f32 gravityScale = 1.0f;        // multiplier on the world's gravity
        f32 linearDamping = 0.0f;       // per-second velocity decay
        f32 angularDamping = 0.0f;      // per-second angular velocity decay

        bool fixedRotation = false;     // when set, the body never rotates

        ~RigidBody2D() override = default;

        // Mass / inertia. A non-positive value is treated as "infinite" (inverse 0),
        // which makes the body immovable along that degree of freedom.
        void SetMass(f32 mass);
        void SetInertia(f32 inertia);

        KIWI_NODISCARD f32 GetMass() const { return m_mass; }
        KIWI_NODISCARD f32 GetInertia() const { return m_inertia; }
        KIWI_NODISCARD f32 GetInverseMass() const { return m_inverseMass; }
        KIWI_NODISCARD f32 GetInverseInertia() const { return m_inverseInertia; }

        // Continuous influences accumulated over the frame and consumed by the next
        // integration step.
        void ApplyForce(const Vec2& force);
        void ApplyTorque(f32 torque);

        // Instantaneous changes applied directly to the velocities.
        void ApplyImpulse(const Vec2& impulse);
        void ApplyAngularImpulse(f32 impulse);

    private:
        friend class PhysicsWorld2D;

        // Whether this body is integrated by the physics world this step.
        KIWI_NODISCARD bool IsSimulated() const { return type != EBodyType::Static; }

        // The accumulated force/torque used for one step is cleared afterwards.
        void ClearAccumulators();

    private:
        f32 m_mass = 1.0f;
        f32 m_inverseMass = 1.0f;
        f32 m_inertia = 1.0f;
        f32 m_inverseInertia = 1.0f;

        Vec2 m_force{ 0.0f, 0.0f };
        f32 m_torque = 0.0f;
    };
}
