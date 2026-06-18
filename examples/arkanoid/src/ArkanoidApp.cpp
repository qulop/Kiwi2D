#include "ArkanoidApp.hpp"

#include "Components.hpp"

#include <core/Engine.hpp>

#include <renderer/Renderer.hpp>
#include <renderer/Camera2D.hpp>

#include <scene/Scene.hpp>
#include <scene/Entity.hpp>
#include <scene/components/Transform2D.hpp>

#include <input/InputSubsystem.hpp>
#include <input/KeyCodes.hpp>

#include <gui/ImGuiSubsystem.hpp>

#include <common/Time.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <algorithm>
#include <random>


namespace Arkanoid {
    namespace {
        constexpr f32 PaddleY = 48.0f;
        constexpr f32 PaddleHalfW = 70.0f;
        constexpr f32 PaddleHalfH = 14.0f;

        constexpr f32 WallThickness = 16.0f;

        // Builds a TRS matrix for an axis-aligned quad centred at `center` with the
        // given full `size`. The renderer's base quad is a unit square at the origin.
        glm::mat4 MakeQuadTransform(const Vec2& center, const Vec2& size) {
            glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0.0f));
            return glm::scale(t, glm::vec3(size.x, size.y, 1.0f));
        }

        f32 RandomBetween(f32 a, f32 b) {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_real_distribution<f32> dist(a, b);
            return dist(rng);
        }
    }


    bool ArkanoidApp::Init() {
        if (!Super::Init()) {
            return false;
        }

        m_scene = std::make_shared<Scene>();

        // Map the logical world (0..W, 0..H) directly onto the camera. y grows upward.
        m_camera = std::make_shared<Camera2D>(0.0f, WorldWidth, 0.0f, WorldHeight);

        // Debug/HUD overlay. The GL context is already current at this point, so the
        // ImGui GL3 + GLFW backends initialize cleanly.
        RegisterSubsystem<Editor::ImGuiSubsystem>();
        m_imgui = GetSubsystem<Editor::ImGuiSubsystem>();
        if (!m_imgui->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the ImGui subsystem");
            return false;
        }

        BuildLevel();

        return true;
    }

    void ArkanoidApp::BuildLevel() {
        m_score = 0;
        m_lives = 3;
        m_state = EGameState::Ready;

        // Paddle
        m_paddle = m_scene->CreateEntity("Paddle");
        auto* paddleTransform = m_paddle->AddComponent<Transform2D>();
        paddleTransform->position = Vec2{ WorldWidth * 0.5f, PaddleY };
        paddleTransform->scale = Vec2{ PaddleHalfW * 2.0f, PaddleHalfH * 2.0f };
        auto* paddle = m_paddle->AddComponent<PaddleComponent>();
        paddle->halfWidth = PaddleHalfW;

        // Ball
        m_ball = m_scene->CreateEntity("Ball");
        auto* ballTransform = m_ball->AddComponent<Transform2D>();
        auto* ball = m_ball->AddComponent<BallComponent>();
        ballTransform->scale = Vec2{ ball->radius * 2.0f, ball->radius * 2.0f };
        ResetBall();

        // Bricks grid
        m_bricksRemaining = 0;
        const f32 gridTop = WorldHeight - 90.0f;
        const f32 brickGapX = 12.0f;
        const f32 brickGapY = 12.0f;
        const f32 usableWidth = WorldWidth - 2.0f * WallThickness - 60.0f;
        const f32 brickW = (usableWidth - brickGapX * (BrickColumns - 1)) / BrickColumns;
        const f32 brickH = 30.0f;
        const f32 originX = WallThickness + 30.0f + brickW * 0.5f;

        const Vec4 rowColors[BrickRows] = {
            { 0.91f, 0.30f, 0.24f, 1.0f },
            { 0.90f, 0.49f, 0.13f, 1.0f },
            { 0.95f, 0.77f, 0.06f, 1.0f },
            { 0.18f, 0.80f, 0.44f, 1.0f },
            { 0.20f, 0.60f, 0.86f, 1.0f },
            { 0.61f, 0.35f, 0.71f, 1.0f },
        };

        for (i32 row = 0; row < BrickRows; ++row) {
            for (i32 col = 0; col < BrickColumns; ++col) {
                Entity* brickEntity = m_scene->CreateEntity("Brick");

                const f32 cx = originX + col * (brickW + brickGapX);
                const f32 cy = gridTop - row * (brickH + brickGapY);

                auto* brickTransform = brickEntity->AddComponent<Transform2D>();
                brickTransform->position = Vec2{ cx, cy };
                brickTransform->scale = Vec2{ brickW, brickH };

                auto* brick = brickEntity->AddComponent<BrickComponent>();
                brick->halfExtents = Vec2{ brickW * 0.5f, brickH * 0.5f };
                brick->color = rowColors[row];
                brick->hitPoints = 1;

                ++m_bricksRemaining;
            }
        }
    }

    void ArkanoidApp::ResetBall() {
        auto* ball = m_ball->GetComponent<BallComponent>();
        auto* ballTransform = m_ball->GetComponent<Transform2D>();
        auto* paddleTransform = m_paddle->GetComponent<Transform2D>();

        ball->launched = false;
        ball->velocity = Vec2{ 0.0f, 0.0f };
        ballTransform->position = Vec2{
            paddleTransform->position.x,
            PaddleY + PaddleHalfH + ball->radius + 2.0f
        };

        m_state = EGameState::Ready;
    }

    void ArkanoidApp::LaunchBall() {
        auto* ball = m_ball->GetComponent<BallComponent>();
        if (ball->launched) {
            return;
        }

        // Launch upward with a slight random horizontal bias.
        const f32 vx = RandomBetween(-0.4f, 0.4f);
        Vec2 dir{ vx, 1.0f };
        dir = dir.GetNormalized();

        ball->velocity = Vec2{ dir.x * ball->baseSpeed, dir.y * ball->baseSpeed };
        ball->launched = true;
        m_state = EGameState::Playing;
    }

    void ArkanoidApp::Update() {
        const f32 dt = std::min(Time::DeltaTime(), 0.033f); // clamp to avoid tunneling on hitches

        auto input = GetSubsystem<InputSubsystem>();
        if (input && input->IsKeyPressed(Key::Escape)) {
            Stop();
        }

        if (m_state == EGameState::Won || m_state == EGameState::GameOver) {
            if (input && input->IsKeyPressed(Key::Enter)) {
                m_scene = std::make_shared<Scene>();
                m_paddle = nullptr;
                m_ball = nullptr;
                BuildLevel();
            }
        }
        else {
            UpdatePaddle(dt);
            UpdateBall(dt);
        }

        m_scene->OnUpdate(dt);

        // Draw the frame here, before Engine::Update() polls input and swaps buffers.
        // Renderer::BeginScene() clears the framebuffer, so this is the only draw site.
        RenderScene();
        RenderHud();
        if (m_imgui) {
            m_imgui->EndFrame();
        }
    }

    void ArkanoidApp::UpdatePaddle(f32 dt) {
        auto input = GetSubsystem<InputSubsystem>();
        if (!input) {
            return;
        }

        auto* paddle = m_paddle->GetComponent<PaddleComponent>();
        auto* transform = m_paddle->GetComponent<Transform2D>();

        f32 dir = 0.0f;
        if (input->IsKeyPressed(Key::Left) || input->IsKeyPressed(Key::A)) {
            dir -= 1.0f;
        }
        if (input->IsKeyPressed(Key::Right) || input->IsKeyPressed(Key::D)) {
            dir += 1.0f;
        }

        transform->position.x += dir * paddle->speed * dt;

        // Clamp inside the side walls.
        const f32 minX = WallThickness + paddle->halfWidth;
        const f32 maxX = WorldWidth - WallThickness - paddle->halfWidth;
        transform->position.x = std::clamp(transform->position.x, minX, maxX);

        if ((m_state == EGameState::Ready) &&
            (input->IsKeyPressed(Key::Space) || input->IsKeyPressed(Key::Up))) {
            LaunchBall();
        }
    }

    void ArkanoidApp::UpdateBall(f32 dt) {
        auto* ball = m_ball->GetComponent<BallComponent>();
        auto* ballTransform = m_ball->GetComponent<Transform2D>();
        auto* paddleTransform = m_paddle->GetComponent<Transform2D>();
        auto* paddle = m_paddle->GetComponent<PaddleComponent>();

        if (!ball->launched) {
            // Stick to the paddle while waiting to launch.
            ballTransform->position.x = paddleTransform->position.x;
            return;
        }

        Vec2 pos = ballTransform->position;
        pos.x += ball->velocity.x * dt;
        pos.y += ball->velocity.y * dt;

        const f32 r = ball->radius;

        // Side + top walls.
        if (pos.x - r <= WallThickness) {
            pos.x = WallThickness + r;
            ball->velocity.x = std::abs(ball->velocity.x);
        }
        else if (pos.x + r >= WorldWidth - WallThickness) {
            pos.x = WorldWidth - WallThickness - r;
            ball->velocity.x = -std::abs(ball->velocity.x);
        }
        if (pos.y + r >= WorldHeight - WallThickness) {
            pos.y = WorldHeight - WallThickness - r;
            ball->velocity.y = -std::abs(ball->velocity.y);
        }

        // Fell below the bottom edge -> lose a life.
        if (pos.y - r <= 0.0f) {
            --m_lives;
            if (m_lives <= 0) {
                m_state = EGameState::GameOver;
            }
            ballTransform->position = pos;
            ResetBall();
            return;
        }

        // Paddle collision (ball moving down only).
        const f32 paddleTop = paddleTransform->position.y + PaddleHalfH;
        if (ball->velocity.y < 0.0f &&
            pos.y - r <= paddleTop &&
            pos.y >= paddleTransform->position.y &&
            std::abs(pos.x - paddleTransform->position.x) <= paddle->halfWidth + r) {

            pos.y = paddleTop + r;

            // Reflect with an angle based on where it hit the paddle.
            const f32 offset = (pos.x - paddleTransform->position.x) / paddle->halfWidth;
            const f32 bounceAngle = offset * 1.0472f; // up to ~60 degrees
            const f32 speed = ball->baseSpeed;
            ball->velocity.x = std::sin(bounceAngle) * speed;
            ball->velocity.y = std::cos(bounceAngle) * speed;
        }

        // Brick collisions (axis-aligned, resolve against the nearer axis).
        for (const std::shared_ptr<Entity>& entity : m_scene->GetEntities()) {
            auto* brick = entity->GetComponent<BrickComponent>();
            if (!brick || !brick->alive) {
                continue;
            }

            auto* brickTransform = entity->GetComponent<Transform2D>();
            const Vec2 bc = brickTransform->position;
            const Vec2 he = brick->halfExtents;

            const f32 dx = pos.x - bc.x;
            const f32 dy = pos.y - bc.y;
            const f32 overlapX = (he.x + r) - std::abs(dx);
            const f32 overlapY = (he.y + r) - std::abs(dy);

            if (overlapX > 0.0f && overlapY > 0.0f) {
                if (overlapX < overlapY) {
                    pos.x += (dx < 0.0f ? -overlapX : overlapX);
                    ball->velocity.x = -ball->velocity.x;
                }
                else {
                    pos.y += (dy < 0.0f ? -overlapY : overlapY);
                    ball->velocity.y = -ball->velocity.y;
                }

                if (--brick->hitPoints <= 0) {
                    brick->alive = false;
                    m_scene->DestroyEntity(entity.get());
                    --m_bricksRemaining;
                    m_score += 100;

                    if (m_bricksRemaining <= 0) {
                        m_state = EGameState::Won;
                    }
                }
                break; // one brick per frame keeps the response stable
            }
        }

        ballTransform->position = pos;
    }

    SharedPtr<Kiwi::Renderer> ArkanoidApp::Renderer() const {
        return GetEngine()->GetRenderer();
    }

    void ArkanoidApp::BeforeFrameBegin() {
        if (m_imgui) {
            m_imgui->BeginFrame();
        }
    }

    void ArkanoidApp::RenderScene() {
        auto renderer = Renderer();
        if (!renderer) {
            return;
        }

        renderer->BeginScene(*m_camera);

        // Walls (subtle frame).
        const Vec4 wallColor{ 0.25f, 0.27f, 0.32f, 1.0f };
        renderer->SubmitDraw(MakeQuadTransform({ WallThickness * 0.5f, WorldHeight * 0.5f },
                                               { WallThickness, WorldHeight }), wallColor);
        renderer->SubmitDraw(MakeQuadTransform({ WorldWidth - WallThickness * 0.5f, WorldHeight * 0.5f },
                                               { WallThickness, WorldHeight }), wallColor);
        renderer->SubmitDraw(MakeQuadTransform({ WorldWidth * 0.5f, WorldHeight - WallThickness * 0.5f },
                                               { WorldWidth, WallThickness }), wallColor);

        // Bricks.
        for (const std::shared_ptr<Entity>& entity : m_scene->GetEntities()) {
            auto* brick = entity->GetComponent<BrickComponent>();
            if (!brick || !brick->alive) {
                continue;
            }
            auto* t = entity->GetComponent<Transform2D>();
            renderer->SubmitDraw(MakeQuadTransform(t->position, t->scale), brick->color);
        }

        // Paddle.
        {
            auto* t = m_paddle->GetComponent<Transform2D>();
            renderer->SubmitDraw(MakeQuadTransform(t->position, t->scale),
                                 Vec4{ 0.95f, 0.95f, 0.98f, 1.0f });
        }

        // Ball.
        {
            auto* t = m_ball->GetComponent<Transform2D>();
            renderer->SubmitDraw(MakeQuadTransform(t->position, t->scale),
                                 Vec4{ 1.0f, 0.85f, 0.30f, 1.0f });
        }

        renderer->EndScene();
    }

    void ArkanoidApp::RenderHud() {
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f) {
            return; // ImGui not yet ready this frame
        }

        ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.55f);
        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("Arkanoid", nullptr, flags)) {
            ImGui::Text("Score: %d", m_score);
            ImGui::Text("Lives: %d", m_lives);
            ImGui::Text("Bricks left: %d", m_bricksRemaining);
            ImGui::Separator();

            switch (m_state) {
                case EGameState::Ready:
                    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.4f, 1.0f),
                        "SPACE / UP: launch   |   A/D or Arrows: move");
                    break;
                case EGameState::Playing:
                    ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.6f, 1.0f), "Playing...");
                    break;
                case EGameState::Won:
                    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.5f, 1.0f),
                        "YOU WIN!  Press ENTER to play again.");
                    break;
                case EGameState::GameOver:
                    ImGui::TextColored(ImVec4(0.95f, 0.4f, 0.35f, 1.0f),
                        "GAME OVER.  Press ENTER to restart.");
                    break;
            }
            ImGui::Text("ESC: quit");
        }
        ImGui::End();
    }
}
