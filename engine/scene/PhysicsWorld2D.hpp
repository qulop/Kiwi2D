#pragma once

#include <core/Object.hpp>

#include <math/vec/Vec2.hpp>


namespace Kiwi {
    class Scene;


    // Minimal 2D physics integrator. Owns the global gravity and advances every
    // entity that carries both a RigidBody2D and a Transform2D using semi-implicit
    // Euler integration. Collision resolution is intentionally out of scope for now;
    // this only handles free-body motion.
    class PhysicsWorld2D : public AObject {
        KIWI_CREATE_OBJECT(PhysicsWorld2D, AObject);

    public:
        PhysicsWorld2D() = default;
        ~PhysicsWorld2D() override = default;

        // Integrates all simulated bodies in the scene over `deltaTime` seconds.
        void Step(Scene& scene, f32 deltaTime);

        void SetGravity(const Vec2& gravity) { m_gravity = gravity; }
        KIWI_NODISCARD const Vec2& GetGravity() const { return m_gravity; }

    private:
        // Earth-like default, pointing down. y grows upward in the 2D world, so the
        // acceleration is negative. Callers working in pixel space will usually want
        // a much larger magnitude via SetGravity().
        Vec2 m_gravity{ 0.0f, -9.81f };
    };
}
