#pragma once

#include <core/Application.hpp>

#include <math/vec/Vec2.hpp>

#include <deque>
#include <memory>


namespace Kiwi {
    class Scene;
    class Camera2D;
    class Renderer;
    class ATexture2D;
}

namespace Kiwi::Editor {
    class ImGuiSubsystem;
}


namespace Snake {
    using namespace Kiwi;


    enum class EGameState {
        Ready,      // waiting for the first input
        Playing,
        GameOver,
        Won         // the snake filled the whole board
    };


    enum class EDirection {
        Up,
        Down,
        Left,
        Right
    };


    // Integer tile coordinate on the play grid.
    struct Cell {
        i32 x = 0;
        i32 y = 0;

        constexpr bool operator==(const Cell& other) const {
            return x == other.x && y == other.y;
        }
    };


    // Classic Snake on the Kiwi2D engine. The board is a fixed tile grid; the snake
    // advances one cell per fixed timestep, growing when it eats food.
    class SnakeApp : public Application {
    public:
        using Super = Application;

        static constexpr i32 GridCols = 24;
        static constexpr i32 GridRows = 18;
        static constexpr f32 TileSize = 32.0f;

    public:
        SnakeApp() = default;
        ~SnakeApp() override = default;

        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;  // ImGui new frame
        void Update() override;            // gameplay + rendering + HUD (before swap)

    private:
        void ResetGame();
        void HandleInput();
        void StepSnake();
        void SpawnFood();

        KIWI_NODISCARD bool IsOnSnake(const Cell& cell) const;

        // Maps a tile to the centre of its quad in world space.
        KIWI_NODISCARD Vec2 TileToWorld(i32 tileX, i32 tileY) const;

        void RenderScene();
        void RenderHud();

        KIWI_NODISCARD SharedPtr<Renderer> Renderer() const;

    private:
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<Camera2D> m_camera;

        SharedPtr<Kiwi::Editor::ImGuiSubsystem> m_imgui;

        SharedPtr<ATexture2D> m_appleTexture;   // food sprite (falls back to a red quad)

        std::deque<Cell> m_body;          // front() is the head
        EDirection m_direction = EDirection::Right;
        EDirection m_pendingDirection = EDirection::Right;

        Cell m_food{ 0, 0 };

        f32 m_stepInterval = 0.13f;       // seconds between moves (speeds up as you grow)
        f32 m_stepTimer = 0.0f;

        i32 m_score = 0;
        i32 m_bestScore = 0;

        f32 m_worldWidth = 0.0f;
        f32 m_worldHeight = 0.0f;

        EGameState m_state = EGameState::Ready;
    };
}
