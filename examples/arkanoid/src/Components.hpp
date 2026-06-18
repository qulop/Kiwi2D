#pragma once

#include <scene/Component.hpp>

#include <math/vec/Vec2.hpp>
#include <math/vec/Vec4.hpp>


namespace Arkanoid {
    using namespace Kiwi;


    // Tag + data for the player-controlled paddle. Movement is driven by ArkanoidApp
    // (which owns the input + world bounds), so this component is mostly data.
    class PaddleComponent : public Component {
        KIWI_CREATE_OBJECT(PaddleComponent, Component);

    public:
        f32 speed = 900.0f;       // world units / second
        f32 halfWidth = 70.0f;    // used for collision against the ball

        ~PaddleComponent() override = default;
    };


    // The bouncing ball. Holds velocity and radius; collision/integration is resolved
    // by the game layer against paddle, bricks and walls.
    class BallComponent : public Component {
        KIWI_CREATE_OBJECT(BallComponent, Component);

    public:
        Vec2 velocity{ 0.0f, 0.0f };
        f32 radius = 12.0f;
        f32 baseSpeed = 560.0f;
        bool launched = false;    // sticks to the paddle until the player launches

        ~BallComponent() override = default;
    };


    // A destructible brick. `alive` is cleared on hit; dead bricks are skipped during
    // rendering and collision and removed from the scene.
    class BrickComponent : public Component {
        KIWI_CREATE_OBJECT(BrickComponent, Component);

    public:
        Vec2 halfExtents{ 50.0f, 18.0f };
        Vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        i32 hitPoints = 1;
        bool alive = true;

        ~BrickComponent() override = default;
    };
}
