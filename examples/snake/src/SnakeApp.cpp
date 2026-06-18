#include "SnakeApp.hpp"

#include <core/Engine.hpp>

#include <renderer/Renderer.hpp>
#include <renderer/Camera2D.hpp>

#include <scene/Scene.hpp>

#include <input/InputSubsystem.hpp>
#include <input/KeyCodes.hpp>

#include <gui/ImGuiSubsystem.hpp>

#include <common/Time.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <algorithm>
#include <random>


namespace Snake {
    namespace {
        glm::mat4 MakeQuadTransform(const Vec2& center, const Vec2& size) {
            glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0.0f));
            return glm::scale(t, glm::vec3(size.x, size.y, 1.0f));
        }

        i32 RandomInt(i32 lo, i32 hi) {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_int_distribution<i32> dist(lo, hi);
            return dist(rng);
        }

        bool IsOpposite(EDirection a, EDirection b) {
            return (a == EDirection::Up && b == EDirection::Down) ||
                   (a == EDirection::Down && b == EDirection::Up) ||
                   (a == EDirection::Left && b == EDirection::Right) ||
                   (a == EDirection::Right && b == EDirection::Left);
        }

        constexpr f32 MinStepInterval = 0.06f;
    }


    bool SnakeApp::Init() {
        if (!Super::Init()) {
            return false;
        }

        m_scene = std::make_shared<Scene>();

        m_worldWidth = GridCols * TileSize;
        m_worldHeight = GridRows * TileSize;
        m_camera = std::make_shared<Camera2D>(0.0f, m_worldWidth, 0.0f, m_worldHeight);

        RegisterSubsystem<Editor::ImGuiSubsystem>();
        m_imgui = GetSubsystem<Editor::ImGuiSubsystem>();
        if (!m_imgui->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the ImGui subsystem");
            return false;
        }

        ResetGame();
        return true;
    }

    void SnakeApp::ResetGame() {
        m_body.clear();

        // Start with a length-3 snake near the centre, heading right.
        const i32 startX = GridCols / 2;
        const i32 startY = GridRows / 2;
        m_body.push_back(Cell{ startX, startY });
        m_body.push_back(Cell{ startX - 1, startY });
        m_body.push_back(Cell{ startX - 2, startY });

        m_direction = EDirection::Right;
        m_pendingDirection = EDirection::Right;

        m_stepInterval = 0.13f;
        m_stepTimer = 0.0f;
        m_score = 0;
        m_state = EGameState::Ready;

        SpawnFood();
    }

    bool SnakeApp::IsOnSnake(const Cell& cell) const {
        return std::any_of(m_body.begin(), m_body.end(),
            [&cell](const Cell& c) { return c == cell; });
    }

    void SnakeApp::SpawnFood() {
        // Pick a random free cell. The board is small, so rejection sampling is fine.
        const i32 freeCells = GridCols * GridRows - static_cast<i32>(m_body.size());
        if (freeCells <= 0) {
            m_state = EGameState::Won;
            return;
        }

        Cell candidate;
        do {
            candidate.x = RandomInt(0, GridCols - 1);
            candidate.y = RandomInt(0, GridRows - 1);
        } while (IsOnSnake(candidate));

        m_food = candidate;
    }

    Vec2 SnakeApp::TileToWorld(i32 tileX, i32 tileY) const {
        return Vec2{ (tileX + 0.5f) * TileSize, (tileY + 0.5f) * TileSize };
    }

    void SnakeApp::HandleInput() {
        auto input = GetSubsystem<InputSubsystem>();
        if (!input) {
            return;
        }

        EDirection requested = m_pendingDirection;
        if (input->IsKeyPressed(Key::Up) || input->IsKeyPressed(Key::W)) {
            requested = EDirection::Up;
        }
        else if (input->IsKeyPressed(Key::Down) || input->IsKeyPressed(Key::S)) {
            requested = EDirection::Down;
        }
        else if (input->IsKeyPressed(Key::Left) || input->IsKeyPressed(Key::A)) {
            requested = EDirection::Left;
        }
        else if (input->IsKeyPressed(Key::Right) || input->IsKeyPressed(Key::D)) {
            requested = EDirection::Right;
        }

        // Can't reverse directly into the neck; queue the turn for the next step.
        if (!IsOpposite(requested, m_direction)) {
            m_pendingDirection = requested;
        }

        if (m_state == EGameState::Ready && requested != m_direction) {
            // Any non-reversing key press also starts the game; but moving in the
            // same starting direction works too once Playing begins.
        }
    }

    void SnakeApp::StepSnake() {
        m_direction = m_pendingDirection;

        Cell head = m_body.front();
        switch (m_direction) {
            case EDirection::Up:    head.y += 1; break;
            case EDirection::Down:  head.y -= 1; break;
            case EDirection::Left:  head.x -= 1; break;
            case EDirection::Right: head.x += 1; break;
        }

        // Wall collision.
        if (head.x < 0 || head.x >= GridCols || head.y < 0 || head.y >= GridRows) {
            m_state = EGameState::GameOver;
            m_bestScore = std::max(m_bestScore, m_score);
            return;
        }

        const bool willEat = (head == m_food);

        // Self collision. The tail cell is about to move out of the way unless we eat,
        // so ignore the current tail when not growing.
        for (size_t i = 0; i < m_body.size(); ++i) {
            if (!willEat && i == m_body.size() - 1) {
                break;
            }
            if (m_body[i] == head) {
                m_state = EGameState::GameOver;
                m_bestScore = std::max(m_bestScore, m_score);
                return;
            }
        }

        m_body.push_front(head);

        if (willEat) {
            m_score += 10;
            // Speed up slightly with every few pieces of food, down to a floor.
            m_stepInterval = std::max(MinStepInterval, m_stepInterval - 0.004f);
            SpawnFood();
        }
        else {
            m_body.pop_back();
        }
    }

    void SnakeApp::Update() {
        const f32 dt = std::min(Time::DeltaTime(), 0.033f);

        auto input = GetSubsystem<InputSubsystem>();
        if (input && input->IsKeyPressed(Key::Escape)) {
            Stop();
        }

        switch (m_state) {
            case EGameState::Ready: {
                HandleInput();
                // Start moving as soon as the player steers.
                if (input && (input->IsKeyPressed(Key::Up) || input->IsKeyPressed(Key::Down) ||
                              input->IsKeyPressed(Key::Left) || input->IsKeyPressed(Key::Right) ||
                              input->IsKeyPressed(Key::W) || input->IsKeyPressed(Key::S) ||
                              input->IsKeyPressed(Key::A) || input->IsKeyPressed(Key::D))) {
                    m_state = EGameState::Playing;
                }
                break;
            }
            case EGameState::Playing: {
                HandleInput();
                m_stepTimer += dt;
                while (m_stepTimer >= m_stepInterval && m_state == EGameState::Playing) {
                    m_stepTimer -= m_stepInterval;
                    StepSnake();
                }
                break;
            }
            case EGameState::GameOver:
            case EGameState::Won: {
                if (input && input->IsKeyPressed(Key::Enter)) {
                    ResetGame();
                }
                break;
            }
        }

        m_scene->OnUpdate(dt);

        RenderScene();
        RenderHud();
        if (m_imgui) {
            m_imgui->EndFrame();
        }
    }

    SharedPtr<Kiwi::Renderer> SnakeApp::Renderer() const {
        return GetEngine()->GetRenderer();
    }

    void SnakeApp::BeforeFrameBegin() {
        if (m_imgui) {
            m_imgui->BeginFrame();
        }
    }

    void SnakeApp::RenderScene() {
        auto renderer = Renderer();
        if (!renderer) {
            return;
        }

        renderer->BeginScene(*m_camera);

        // Subtle checkerboard background so the grid is readable.
        const Vec4 dark{ 0.10f, 0.12f, 0.14f, 1.0f };
        const Vec4 light{ 0.13f, 0.16f, 0.18f, 1.0f };
        for (i32 y = 0; y < GridRows; ++y) {
            for (i32 x = 0; x < GridCols; ++x) {
                const Vec4 c = ((x + y) % 2 == 0) ? dark : light;
                renderer->SubmitDraw(
                    MakeQuadTransform(TileToWorld(x, y), Vec2{ TileSize, TileSize }), c);
            }
        }

        // Food.
        renderer->SubmitDraw(
            MakeQuadTransform(TileToWorld(m_food.x, m_food.y), Vec2{ TileSize * 0.6f, TileSize * 0.6f }),
            Vec4{ 0.95f, 0.30f, 0.28f, 1.0f });

        // Snake body. The head is brighter than the rest.
        const Vec4 headColor{ 0.55f, 0.95f, 0.40f, 1.0f };
        const Vec4 bodyColor{ 0.30f, 0.75f, 0.32f, 1.0f };
        for (size_t i = 0; i < m_body.size(); ++i) {
            const Cell& c = m_body[i];
            const Vec4 color = (i == 0) ? headColor : bodyColor;
            const f32 inset = (i == 0) ? 0.88f : 0.78f;
            renderer->SubmitDraw(
                MakeQuadTransform(TileToWorld(c.x, c.y), Vec2{ TileSize * inset, TileSize * inset }),
                color);
        }

        renderer->EndScene();
    }

    void SnakeApp::RenderHud() {
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f) {
            return;
        }

        ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.55f);
        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("Snake", nullptr, flags)) {
            ImGui::Text("Score: %d", m_score);
            ImGui::Text("Best:  %d", m_bestScore);
            ImGui::Text("Length: %d", static_cast<i32>(m_body.size()));
            ImGui::Separator();

            switch (m_state) {
                case EGameState::Ready:
                    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.4f, 1.0f),
                        "WASD / Arrows to start moving");
                    break;
                case EGameState::Playing:
                    ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f), "Eat the red food!");
                    break;
                case EGameState::Won:
                    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.5f, 1.0f),
                        "YOU FILLED THE BOARD!  Press ENTER to play again.");
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
