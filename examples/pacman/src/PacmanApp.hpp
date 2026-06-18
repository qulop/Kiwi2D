#pragma once

#include <core/Application.hpp>

#include "Components.hpp"

#include <vector>
#include <memory>


namespace Kiwi {
    class Scene;
    class Entity;
    class Camera2D;
    class Renderer;
}

namespace Kiwi::Editor {
    class ImGuiSubsystem;
}


namespace Pacman {
    using namespace Kiwi;


    enum class EGameState {
        Ready,      // brief pause before the round starts
        Playing,
        Won,
        GameOver
    };


    // What occupies a maze tile in the static layout.
    enum class ETile : u8 {
        Wall,
        Empty,      // walkable, no pellet (already eaten / spawn lane)
        Pellet,
        Power       // power pellet: frightens the ghosts
    };


    // A classic Pac-Man maze game on the Kiwi2D engine. Actors move on a tile grid;
    // rendering maps the grid into world space via an orthographic Camera2D.
    class PacmanApp : public Application {
    public:
        using Super = Application;

        static constexpr f32 TileSize = 32.0f;  // world units per tile

    public:
        PacmanApp() = default;
        ~PacmanApp() override = default;

        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;  // ImGui new frame
        void Update() override;            // gameplay + rendering + HUD (before swap)

    private:
        void BuildLevel();
        void ResetActors();

        void UpdatePacman(f32 dt);
        void UpdateGhost(Entity* ghostEntity, f32 dt);
        void StepMover(GridMover& mover, f32 dt, bool isPlayer);

        KIWI_NODISCARD bool IsWalkable(i32 tileX, i32 tileY) const;
        KIWI_NODISCARD ETile& TileAt(i32 tileX, i32 tileY);
        KIWI_NODISCARD ETile TileAt(i32 tileX, i32 tileY) const;

        // Tile <-> world-space conversion. The maze is centred horizontally and the
        // first row sits near the top of the play field.
        KIWI_NODISCARD Vec2 TileToWorld(f32 tileX, f32 tileY) const;

        void FrightenGhosts();
        void HandleGhostCollision(Entity* ghostEntity);

        void RenderScene();
        void RenderHud();

        KIWI_NODISCARD SharedPtr<Renderer> Renderer() const;

    private:
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<Camera2D> m_camera;

        SharedPtr<Kiwi::Editor::ImGuiSubsystem> m_imgui;

        Entity* m_pacman = nullptr;
        std::vector<Entity*> m_ghosts;

        std::vector<ETile> m_tiles;
        i32 m_cols = 0;
        i32 m_rows = 0;

        i32 m_pelletsRemaining = 0;
        i32 m_score = 0;
        i32 m_lives = 3;
        f32 m_readyTimer = 0.0f;

        f32 m_worldWidth = 0.0f;
        f32 m_worldHeight = 0.0f;

        EGameState m_state = EGameState::Ready;
    };
}
