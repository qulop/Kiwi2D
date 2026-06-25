#pragma once

#include <core/Application.hpp>

#include <math/vec/Vec2.hpp>

#include <memory>


namespace Kiwi {
    class Scene;
    class Entity;
    class Camera2D;
    class Renderer;
    class ATexture2D;
}

namespace Kiwi::Editor {
    class ImGuiSubsystem;
}


namespace Arkanoid {
    using namespace Kiwi;


    enum class EGameState {
        Ready,      // ball stuck to paddle, waiting for launch
        Playing,
        Won,
        GameOver
    };


    // The Arkanoid game built on the Kiwi2D engine. It owns a Scene (ECS), an
    // orthographic Camera2D, and drives gameplay from the Application frame hooks.
    class ArkanoidApp : public Application {
    public:
        using Super = Application;

        // Fixed logical play-field size (world units). The camera maps this 1:1.
        static constexpr f32 WorldWidth = 1280.0f;
        static constexpr f32 WorldHeight = 720.0f;

        static constexpr i32 BrickColumns = 11;
        static constexpr i32 BrickRows = 6;

    public:
        ArkanoidApp() = default;
        ~ArkanoidApp() override = default;

        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;  // ImGui new frame
        void Update() override;            // gameplay tick + rendering + HUD (before buffer swap)

    private:
        void BuildLevel();
        void ResetBall();
        void LaunchBall();

        void UpdatePaddle(f32 dt);
        void UpdateBall(f32 dt);

        void RenderScene();
        void RenderHud();

        KIWI_NODISCARD SharedPtr<Renderer> Renderer() const;

    private:
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<Camera2D> m_camera;

        SharedPtr<Kiwi::Editor::ImGuiSubsystem> m_imgui;

        // Procedurally generated disc texture so the ball renders as an actual circle
        // (the batch renderer otherwise only draws quads).
        SharedPtr<ATexture2D> m_ballTexture;

        Entity* m_paddle = nullptr;
        Entity* m_ball = nullptr;

        EGameState m_state = EGameState::Ready;
        i32 m_score = 0;
        i32 m_lives = 3;
        i32 m_bricksRemaining = 0;
    };
}
