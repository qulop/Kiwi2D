#pragma once

#include <scene/Component.hpp>

#include <math/vec/Vec2.hpp>
#include <math/vec/Vec4.hpp>


namespace Pacman {
    using namespace Kiwi;


    // Grid-aligned movement direction. None means "stopped / no queued turn".
    enum class EDirection {
        None,
        Up,
        Down,
        Left,
        Right
    };


    // Common state shared by every grid-walking actor (player and ghosts). Actors live
    // on integer tile coordinates and interpolate smoothly between the current tile and
    // the next one as they move; `progress` is the 0..1 fraction along that step.
    class GridMover : public Component {
        KIWI_CREATE_OBJECT(GridMover, Component);

    public:
        i32 tileX = 0;            // current tile (the one being left)
        i32 tileY = 0;
        i32 nextTileX = 0;        // tile being entered
        i32 nextTileY = 0;
        f32 progress = 0.0f;      // 0..1 between (tileX,tileY) and (nextTileX,nextTileY)
        f32 speed = 6.0f;         // tiles per second

        EDirection direction = EDirection::None;
        EDirection desiredDirection = EDirection::None;

        ~GridMover() override = default;
    };


    // The player. Just a tag with a small amount of extra state; movement is driven by
    // the GridMover above plus player input handled in PacmanApp.
    class PacmanComponent : public Component {
        KIWI_CREATE_OBJECT(PacmanComponent, Component);

    public:
        f32 mouthTimer = 0.0f;    // drives the chomp animation
        bool mouthOpen = true;

        ~PacmanComponent() override = default;
    };


    // A ghost enemy. Each ghost has its own colour and a simple scatter/chase behaviour.
    class GhostComponent : public Component {
        KIWI_CREATE_OBJECT(GhostComponent, Component);

    public:
        Vec4 color{ 1.0f, 0.0f, 0.0f, 1.0f };
        i32 homeTileX = 0;        // scatter target corner
        i32 homeTileY = 0;
        bool frightened = false;
        f32 frightenedTimer = 0.0f;
        f32 chaseChance = 0.75f;  // probability of chasing vs. wandering at a junction

        ~GhostComponent() override = default;
    };
}
