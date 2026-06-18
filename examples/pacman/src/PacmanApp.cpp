#include "PacmanApp.hpp"

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
#include <array>
#include <cmath>
#include <random>
#include <string_view>


namespace Pacman {
    namespace {
        // Classic-style maze. '#' wall, '.' pellet, 'o' power pellet, ' ' empty lane,
        // 'P' player spawn, 'G' ghost spawn. All rows must be the same width.
        constexpr std::array<std::string_view, 21> MAZE = {
            "###################",
            "#........#........#",
            "#o##.###.#.###.##o#",
            "#.................#",
            "#.##.#.#####.#.##.#",
            "#....#...#...#....#",
            "####.### # ###.####",
            "   #.#   G   #.#   ",
            "####.# ## ## #.####",
            "    .   GGG   .    ",
            "####.# ##### #.####",
            "   #.#       #.#   ",
            "####.# ##### #.####",
            "#........#........#",
            "#.##.###.#.###.##.#",
            "#o.#.....P.....#.o#",
            "##.#.#.#####.#.#.##",
            "#....#...#...#....#",
            "#.######.#.######.#",
            "#.................#",
            "###################",
        };

        glm::mat4 MakeQuadTransform(const Vec2& center, const Vec2& size) {
            glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0.0f));
            return glm::scale(t, glm::vec3(size.x, size.y, 1.0f));
        }

        i32 RandomInt(i32 lo, i32 hi) {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_int_distribution<i32> dist(lo, hi);
            return dist(rng);
        }

        f32 RandomUnit() {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_real_distribution<f32> dist(0.0f, 1.0f);
            return dist(rng);
        }

        struct Offset { i32 dx; i32 dy; };

        Offset DirOffset(EDirection dir) {
            switch (dir) {
                case EDirection::Up:    return { 0, +1 };
                case EDirection::Down:  return { 0, -1 };
                case EDirection::Left:  return { -1, 0 };
                case EDirection::Right: return { +1, 0 };
                default:                return { 0, 0 };
            }
        }

        EDirection Opposite(EDirection dir) {
            switch (dir) {
                case EDirection::Up:    return EDirection::Down;
                case EDirection::Down:  return EDirection::Up;
                case EDirection::Left:  return EDirection::Right;
                case EDirection::Right: return EDirection::Left;
                default:                return EDirection::None;
            }
        }

        constexpr f32 ReadyDuration = 1.5f;
        constexpr f32 FrightenedDuration = 7.0f;
    }


    bool PacmanApp::Init() {
        if (!Super::Init()) {
            return false;
        }

        m_scene = std::make_shared<Scene>();

        BuildLevel();

        // World maps the maze 1:1; centre the camera on the maze.
        m_camera = std::make_shared<Camera2D>(0.0f, m_worldWidth, 0.0f, m_worldHeight);

        RegisterSubsystem<Editor::ImGuiSubsystem>();
        m_imgui = GetSubsystem<Editor::ImGuiSubsystem>();
        if (!m_imgui->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the ImGui subsystem");
            return false;
        }

        return true;
    }

    void PacmanApp::BuildLevel() {
        m_rows = static_cast<i32>(MAZE.size());
        m_cols = static_cast<i32>(MAZE[0].size());

        m_worldWidth = m_cols * TileSize;
        m_worldHeight = m_rows * TileSize;

        m_tiles.assign(static_cast<size_t>(m_cols * m_rows), ETile::Empty);
        m_pelletsRemaining = 0;
        m_score = 0;
        m_lives = 3;

        m_ghosts.clear();
        m_pacman = nullptr;

        i32 pacSpawnX = 1, pacSpawnY = 1;
        std::vector<std::pair<i32, i32>> ghostSpawns;

        // The maze string has row 0 at the top; flip Y so tile (0,0) is the bottom-left
        // in world space (the camera's y grows upward).
        for (i32 r = 0; r < m_rows; ++r) {
            const std::string_view row = MAZE[static_cast<size_t>(r)];
            const i32 tileY = (m_rows - 1) - r;

            for (i32 c = 0; c < m_cols; ++c) {
                const char ch = c < static_cast<i32>(row.size()) ? row[static_cast<size_t>(c)] : '#';
                ETile& tile = TileAt(c, tileY);

                switch (ch) {
                    case '#':
                    case '=': // ghost-house door, treated as a wall for the player
                        tile = ETile::Wall;
                        break;
                    case '.':
                        tile = ETile::Pellet;
                        ++m_pelletsRemaining;
                        break;
                    case 'o':
                        tile = ETile::Power;
                        ++m_pelletsRemaining;
                        break;
                    case 'P':
                        tile = ETile::Empty;
                        pacSpawnX = c;
                        pacSpawnY = tileY;
                        break;
                    case 'G':
                        tile = ETile::Empty;
                        ghostSpawns.emplace_back(c, tileY);
                        break;
                    default:
                        tile = ETile::Empty;
                        break;
                }
            }
        }

        // Player entity.
        m_pacman = m_scene->CreateEntity("Pacman");
        m_pacman->AddComponent<Transform2D>();
        m_pacman->AddComponent<PacmanComponent>();
        auto* pacMover = m_pacman->AddComponent<GridMover>();
        pacMover->tileX = pacMover->nextTileX = pacSpawnX;
        pacMover->tileY = pacMover->nextTileY = pacSpawnY;
        pacMover->speed = 6.5f;

        // Ghosts. Colours follow the arcade originals (Blinky/Pinky/Inky/Clyde).
        const Vec4 ghostColors[4] = {
            { 0.96f, 0.20f, 0.18f, 1.0f }, // red
            { 0.98f, 0.62f, 0.78f, 1.0f }, // pink
            { 0.20f, 0.85f, 0.92f, 1.0f }, // cyan
            { 0.98f, 0.62f, 0.20f, 1.0f }, // orange
        };
        const std::pair<i32, i32> homeCorners[4] = {
            { m_cols - 2, m_rows - 2 },
            { 1, m_rows - 2 },
            { m_cols - 2, 1 },
            { 1, 1 },
        };

        if (ghostSpawns.empty()) {
            ghostSpawns.emplace_back(m_cols / 2, m_rows / 2);
        }

        const i32 ghostCount = 4;
        for (i32 i = 0; i < ghostCount; ++i) {
            Entity* ghost = m_scene->CreateEntity("Ghost");
            ghost->AddComponent<Transform2D>();

            auto* gc = ghost->AddComponent<GhostComponent>();
            gc->color = ghostColors[i % 4];
            gc->homeTileX = homeCorners[i % 4].first;
            gc->homeTileY = homeCorners[i % 4].second;
            gc->chaseChance = 0.6f + 0.1f * static_cast<f32>(i);

            const auto [sx, sy] = ghostSpawns[static_cast<size_t>(i) % ghostSpawns.size()];
            auto* gm = ghost->AddComponent<GridMover>();
            gm->tileX = gm->nextTileX = sx;
            gm->tileY = gm->nextTileY = sy;
            gm->speed = 5.0f + 0.25f * static_cast<f32>(i);
            gm->direction = EDirection::Up;

            m_ghosts.push_back(ghost);
        }

        m_state = EGameState::Ready;
        m_readyTimer = ReadyDuration;
    }

    void PacmanApp::ResetActors() {
        // Re-place actors at their spawns without rebuilding pellets (used after a death).
        // Spawns are recovered from the maze definition.
        i32 r = 0;
        std::vector<std::pair<i32, i32>> ghostSpawns;
        i32 pacX = 1, pacY = 1;

        for (const std::string_view row : MAZE) {
            const i32 tileY = (m_rows - 1) - r;
            for (i32 c = 0; c < static_cast<i32>(row.size()); ++c) {
                if (row[static_cast<size_t>(c)] == 'P') {
                    pacX = c;
                    pacY = tileY;
                }
                else if (row[static_cast<size_t>(c)] == 'G') {
                    ghostSpawns.emplace_back(c, tileY);
                }
            }
            ++r;
        }

        auto* pacMover = m_pacman->GetComponent<GridMover>();
        pacMover->tileX = pacMover->nextTileX = pacX;
        pacMover->tileY = pacMover->nextTileY = pacY;
        pacMover->progress = 0.0f;
        pacMover->direction = EDirection::None;
        pacMover->desiredDirection = EDirection::None;

        for (size_t i = 0; i < m_ghosts.size(); ++i) {
            auto* gm = m_ghosts[i]->GetComponent<GridMover>();
            auto* gc = m_ghosts[i]->GetComponent<GhostComponent>();

            const auto [sx, sy] = ghostSpawns.empty()
                ? std::pair<i32, i32>{ m_cols / 2, m_rows / 2 }
                : ghostSpawns[i % ghostSpawns.size()];

            gm->tileX = gm->nextTileX = sx;
            gm->tileY = gm->nextTileY = sy;
            gm->progress = 0.0f;
            gm->direction = EDirection::Up;
            gc->frightened = false;
            gc->frightenedTimer = 0.0f;
        }

        m_state = EGameState::Ready;
        m_readyTimer = ReadyDuration;
    }

    ETile& PacmanApp::TileAt(i32 tileX, i32 tileY) {
        return m_tiles[static_cast<size_t>(tileY * m_cols + tileX)];
    }

    ETile PacmanApp::TileAt(i32 tileX, i32 tileY) const {
        return m_tiles[static_cast<size_t>(tileY * m_cols + tileX)];
    }

    bool PacmanApp::IsWalkable(i32 tileX, i32 tileY) const {
        if (tileX < 0 || tileX >= m_cols || tileY < 0 || tileY >= m_rows) {
            return false;
        }
        return TileAt(tileX, tileY) != ETile::Wall;
    }

    Vec2 PacmanApp::TileToWorld(f32 tileX, f32 tileY) const {
        return Vec2{ (tileX + 0.5f) * TileSize, (tileY + 0.5f) * TileSize };
    }

    void PacmanApp::Update() {
        const f32 dt = std::min(Time::DeltaTime(), 0.033f);

        auto input = GetSubsystem<InputSubsystem>();
        if (input && input->IsKeyPressed(Key::Escape)) {
            Stop();
        }

        switch (m_state) {
            case EGameState::Ready: {
                m_readyTimer -= dt;
                if (m_readyTimer <= 0.0f) {
                    m_state = EGameState::Playing;
                }
                break;
            }
            case EGameState::Playing: {
                UpdatePacman(dt);
                for (Entity* ghost : m_ghosts) {
                    UpdateGhost(ghost, dt);
                }
                break;
            }
            case EGameState::Won:
            case EGameState::GameOver: {
                if (input && input->IsKeyPressed(Key::Enter)) {
                    m_scene = std::make_shared<Scene>();
                    BuildLevel();
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

    void PacmanApp::StepMover(GridMover& mover, f32 dt, bool isPlayer) {
        // When sitting exactly on a tile, decide the next tile to move into.
        if (mover.progress <= 0.0f) {
            mover.tileX = mover.nextTileX;
            mover.tileY = mover.nextTileY;

            // The player may queue a turn; honour it as soon as that way is open.
            if (isPlayer && mover.desiredDirection != EDirection::None) {
                const Offset d = DirOffset(mover.desiredDirection);
                if (IsWalkable(mover.tileX + d.dx, mover.tileY + d.dy)) {
                    mover.direction = mover.desiredDirection;
                }
            }

            const Offset off = DirOffset(mover.direction);
            const i32 tx = mover.tileX + off.dx;
            const i32 ty = mover.tileY + off.dy;

            if (mover.direction != EDirection::None && IsWalkable(tx, ty)) {
                mover.nextTileX = tx;
                mover.nextTileY = ty;
            }
            else {
                // Blocked: stay put this step.
                mover.nextTileX = mover.tileX;
                mover.nextTileY = mover.tileY;
                if (isPlayer) {
                    mover.direction = EDirection::None;
                }
                return;
            }
        }

        mover.progress += mover.speed * dt;
        while (mover.progress >= 1.0f) {
            mover.progress -= 1.0f;
            mover.tileX = mover.nextTileX;
            mover.tileY = mover.nextTileY;

            if (isPlayer && mover.desiredDirection != EDirection::None) {
                const Offset d = DirOffset(mover.desiredDirection);
                if (IsWalkable(mover.tileX + d.dx, mover.tileY + d.dy)) {
                    mover.direction = mover.desiredDirection;
                }
            }

            const Offset off = DirOffset(mover.direction);
            const i32 tx = mover.tileX + off.dx;
            const i32 ty = mover.tileY + off.dy;
            if (mover.direction != EDirection::None && IsWalkable(tx, ty)) {
                mover.nextTileX = tx;
                mover.nextTileY = ty;
            }
            else {
                mover.nextTileX = mover.tileX;
                mover.nextTileY = mover.tileY;
                mover.progress = 0.0f;
                if (isPlayer) {
                    mover.direction = EDirection::None;
                }
                break;
            }
        }
    }

    void PacmanApp::UpdatePacman(f32 dt) {
        auto input = GetSubsystem<InputSubsystem>();
        auto* mover = m_pacman->GetComponent<GridMover>();
        auto* pac = m_pacman->GetComponent<PacmanComponent>();

        if (input) {
            if (input->IsKeyPressed(Key::Up) || input->IsKeyPressed(Key::W)) {
                mover->desiredDirection = EDirection::Up;
            }
            else if (input->IsKeyPressed(Key::Down) || input->IsKeyPressed(Key::S)) {
                mover->desiredDirection = EDirection::Down;
            }
            else if (input->IsKeyPressed(Key::Left) || input->IsKeyPressed(Key::A)) {
                mover->desiredDirection = EDirection::Left;
            }
            else if (input->IsKeyPressed(Key::Right) || input->IsKeyPressed(Key::D)) {
                mover->desiredDirection = EDirection::Right;
            }
        }

        // Allow an immediate start from rest if a direction is queued.
        if (mover->direction == EDirection::None && mover->desiredDirection != EDirection::None) {
            const Offset d = DirOffset(mover->desiredDirection);
            if (IsWalkable(mover->tileX + d.dx, mover->tileY + d.dy)) {
                mover->direction = mover->desiredDirection;
            }
        }

        StepMover(*mover, dt, /*isPlayer=*/true);

        // Eat whatever pellet is on the tile the player currently occupies.
        const i32 cx = mover->progress < 0.5f ? mover->tileX : mover->nextTileX;
        const i32 cy = mover->progress < 0.5f ? mover->tileY : mover->nextTileY;
        ETile& tile = TileAt(cx, cy);
        if (tile == ETile::Pellet) {
            tile = ETile::Empty;
            m_score += 10;
            --m_pelletsRemaining;
        }
        else if (tile == ETile::Power) {
            tile = ETile::Empty;
            m_score += 50;
            --m_pelletsRemaining;
            FrightenGhosts();
        }

        if (m_pelletsRemaining <= 0) {
            m_state = EGameState::Won;
        }

        // Chomp animation.
        pac->mouthTimer += dt;
        if (pac->mouthTimer >= 0.1f) {
            pac->mouthTimer = 0.0f;
            pac->mouthOpen = !pac->mouthOpen;
        }

        // Ghost collisions resolved here too (player may walk into a ghost).
        for (Entity* ghost : m_ghosts) {
            HandleGhostCollision(ghost);
        }
    }

    void PacmanApp::UpdateGhost(Entity* ghostEntity, f32 dt) {
        auto* mover = ghostEntity->GetComponent<GridMover>();
        auto* ghost = ghostEntity->GetComponent<GhostComponent>();
        auto* pacMover = m_pacman->GetComponent<GridMover>();

        if (ghost->frightened) {
            ghost->frightenedTimer -= dt;
            if (ghost->frightenedTimer <= 0.0f) {
                ghost->frightened = false;
            }
        }

        // Choose a direction only at tile centres (junction decisions).
        if (mover->progress <= 0.0f) {
            std::array<EDirection, 4> options = {
                EDirection::Up, EDirection::Down, EDirection::Left, EDirection::Right
            };

            EDirection best = EDirection::None;
            f32 bestScore = ghost->frightened ? -1.0f : 1e9f;

            const i32 targetX = ghost->frightened ? ghost->homeTileX : pacMover->tileX;
            const i32 targetY = ghost->frightened ? ghost->homeTileY : pacMover->tileY;

            const bool wander = !ghost->frightened && RandomUnit() > ghost->chaseChance;

            std::vector<EDirection> walkable;
            for (EDirection dir : options) {
                if (dir == Opposite(mover->direction)) {
                    continue; // ghosts don't reverse unless they must
                }
                const Offset off = DirOffset(dir);
                if (!IsWalkable(mover->tileX + off.dx, mover->tileY + off.dy)) {
                    continue;
                }
                walkable.push_back(dir);

                const i32 nx = mover->tileX + off.dx;
                const i32 ny = mover->tileY + off.dy;
                const f32 dx = static_cast<f32>(nx - targetX);
                const f32 dy = static_cast<f32>(ny - targetY);
                const f32 dist = dx * dx + dy * dy;

                if (ghost->frightened) {
                    if (dist > bestScore) { bestScore = dist; best = dir; } // flee
                }
                else {
                    if (dist < bestScore) { bestScore = dist; best = dir; } // chase
                }
            }

            if (walkable.empty()) {
                // Dead end: reverse.
                best = Opposite(mover->direction);
            }
            else if (wander) {
                best = walkable[static_cast<size_t>(RandomInt(0, static_cast<i32>(walkable.size()) - 1))];
            }

            if (best != EDirection::None) {
                mover->direction = best;
            }
        }

        StepMover(*mover, dt, /*isPlayer=*/false);

        HandleGhostCollision(ghostEntity);
    }

    void PacmanApp::FrightenGhosts() {
        for (Entity* ghost : m_ghosts) {
            auto* gc = ghost->GetComponent<GhostComponent>();
            gc->frightened = true;
            gc->frightenedTimer = FrightenedDuration;

            // Make them turn tail.
            auto* gm = ghost->GetComponent<GridMover>();
            gm->direction = Opposite(gm->direction);
        }
    }

    void PacmanApp::HandleGhostCollision(Entity* ghostEntity) {
        if (m_state != EGameState::Playing) {
            return;
        }

        auto* pacMover = m_pacman->GetComponent<GridMover>();
        auto* gm = ghostEntity->GetComponent<GridMover>();
        auto* gc = ghostEntity->GetComponent<GhostComponent>();

        // Approximate positions in continuous tile space for an overlap test.
        const Offset pacOff = DirOffset(pacMover->direction);
        const f32 pacX = pacMover->tileX + pacOff.dx * pacMover->progress;
        const f32 pacY = pacMover->tileY + pacOff.dy * pacMover->progress;

        const Offset gOff = DirOffset(gm->direction);
        const f32 gx = gm->tileX + gOff.dx * gm->progress;
        const f32 gy = gm->tileY + gOff.dy * gm->progress;

        const f32 dx = pacX - gx;
        const f32 dy = pacY - gy;
        if (dx * dx + dy * dy > 0.5f * 0.5f) {
            return; // not overlapping
        }

        if (gc->frightened) {
            // Eat the ghost: send it home and score.
            m_score += 200;
            gc->frightened = false;
            gc->frightenedTimer = 0.0f;
            gm->tileX = gm->nextTileX = gc->homeTileX;
            gm->tileY = gm->nextTileY = gc->homeTileY;
            gm->progress = 0.0f;
        }
        else {
            --m_lives;
            if (m_lives <= 0) {
                m_state = EGameState::GameOver;
            }
            else {
                ResetActors();
            }
        }
    }

    SharedPtr<Kiwi::Renderer> PacmanApp::Renderer() const {
        return GetEngine()->GetRenderer();
    }

    void PacmanApp::BeforeFrameBegin() {
        if (m_imgui) {
            m_imgui->BeginFrame();
        }
    }

    void PacmanApp::RenderScene() {
        auto renderer = Renderer();
        if (!renderer) {
            return;
        }

        renderer->BeginScene(*m_camera);

        const Vec4 wallColor{ 0.13f, 0.18f, 0.55f, 1.0f };
        const Vec4 pelletColor{ 0.95f, 0.85f, 0.62f, 1.0f };
        const Vec4 powerColor{ 0.98f, 0.72f, 0.30f, 1.0f };

        // Maze tiles + pellets.
        for (i32 y = 0; y < m_rows; ++y) {
            for (i32 x = 0; x < m_cols; ++x) {
                const ETile tile = TileAt(x, y);
                const Vec2 center = TileToWorld(static_cast<f32>(x), static_cast<f32>(y));

                switch (tile) {
                    case ETile::Wall:
                        renderer->SubmitDraw(
                            MakeQuadTransform(center, Vec2{ TileSize, TileSize }), wallColor);
                        break;
                    case ETile::Pellet:
                        renderer->SubmitDraw(
                            MakeQuadTransform(center, Vec2{ TileSize * 0.18f, TileSize * 0.18f }), pelletColor);
                        break;
                    case ETile::Power:
                        renderer->SubmitDraw(
                            MakeQuadTransform(center, Vec2{ TileSize * 0.45f, TileSize * 0.45f }), powerColor);
                        break;
                    default:
                        break;
                }
            }
        }

        // Ghosts.
        for (Entity* ghost : m_ghosts) {
            auto* gm = ghost->GetComponent<GridMover>();
            auto* gc = ghost->GetComponent<GhostComponent>();

            const Offset off = DirOffset(gm->direction);
            const f32 fx = gm->tileX + off.dx * gm->progress;
            const f32 fy = gm->tileY + off.dy * gm->progress;
            const Vec2 center = TileToWorld(fx, fy);

            Vec4 color = gc->color;
            if (gc->frightened) {
                // Flash near the end of the frightened window.
                const bool flashing = gc->frightenedTimer < 2.0f &&
                    std::fmod(gc->frightenedTimer, 0.4f) < 0.2f;
                color = flashing ? Vec4{ 0.9f, 0.9f, 0.95f, 1.0f } : Vec4{ 0.2f, 0.2f, 0.95f, 1.0f };
            }

            renderer->SubmitDraw(MakeQuadTransform(center, Vec2{ TileSize * 0.8f, TileSize * 0.8f }), color);
        }

        // Pac-Man.
        {
            auto* gm = m_pacman->GetComponent<GridMover>();
            auto* pac = m_pacman->GetComponent<PacmanComponent>();

            const Offset off = DirOffset(gm->direction);
            const f32 fx = gm->tileX + off.dx * gm->progress;
            const f32 fy = gm->tileY + off.dy * gm->progress;
            const Vec2 center = TileToWorld(fx, fy);

            const f32 size = pac->mouthOpen ? TileSize * 0.72f : TileSize * 0.82f;
            renderer->SubmitDraw(MakeQuadTransform(center, Vec2{ size, size }),
                                 Vec4{ 1.0f, 0.92f, 0.20f, 1.0f });
        }

        renderer->EndScene();
    }

    void PacmanApp::RenderHud() {
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

        if (ImGui::Begin("Pac-Man", nullptr, flags)) {
            ImGui::Text("Score: %d", m_score);
            ImGui::Text("Lives: %d", m_lives);
            ImGui::Text("Pellets left: %d", m_pelletsRemaining);
            ImGui::Separator();

            switch (m_state) {
                case EGameState::Ready:
                    ImGui::TextColored(ImVec4(0.95f, 0.9f, 0.4f, 1.0f), "Get ready!");
                    break;
                case EGameState::Playing:
                    ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f),
                        "WASD / Arrows to move   |   eat the power pellets!");
                    break;
                case EGameState::Won:
                    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.5f, 1.0f),
                        "LEVEL CLEARED!  Press ENTER to play again.");
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
