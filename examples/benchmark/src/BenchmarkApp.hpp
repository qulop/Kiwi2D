#pragma once

#include <core/Application.hpp>

#include <math/vec/Vec2.hpp>
#include <math/vec/Vec4.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>


namespace Kiwi {
    class Camera2D;
    class Renderer;
    class ATexture2D;
}

namespace Kiwi::Editor {
    class ImGuiSubsystem;
}


namespace Benchmark {
    using namespace Kiwi;


    // One animated quad in the stress field. When `texture` is null the quad is drawn
    // as a flat colour (the original behaviour); otherwise it shows that sprite image.
    struct SpriteInstance {
        Vec2 basePos{ 0.0f, 0.0f };
        Vec2 size{ 0.0f, 0.0f };
        Vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        f32  phase = 0.0f;
        SharedPtr<ATexture2D> texture;
    };


    // A single measured row of the load test (mirrors a row of Таблица 4.4).
    struct BenchResult {
        i32 sprites   = 0;
        i32 drawCalls = 0;
        f32 frameMs   = 0.0f;
        f32 fps       = 0.0f;
        bool valid    = false;
    };


    // Renderer load test for the Kiwi2D batch renderer. Draws a configurable number
    // of quads with VSync disabled and measures the average frame time, derived FPS
    // and the number of GPU draw calls produced by the batching system. A full sweep
    // walks the same sprite counts that are reported in the diploma's load-test table.
    class BenchmarkApp : public Application {
    public:
        using Super = Application;

        // Sprite counts probed by a full sweep — identical to Таблица 4.4.
        static constexpr std::array<i32, 7> kCounts{ 100, 500, 1000, 5000, 10000, 15000, 20000 };

        static constexpr i32 kWarmupFrames  = 30;   // discarded before each measurement
        static constexpr i32 kMeasureFrames = 300;  // averaged window (as in the report)

    public:
        BenchmarkApp() = default;
        ~BenchmarkApp() override = default;

        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;   // ImGui new frame
        void Update() override;             // input + render + overlay

    private:
        void StartSweep();
        void AdvanceSweep(f32 dt);
        void RebuildSprites(i32 count);
        void RenderScene();
        void RenderOverlay();

        KIWI_NODISCARD static i32 ComputeDrawCalls(i32 spriteCount);
        KIWI_NODISCARD SharedPtr<Kiwi::Renderer> Renderer() const;

    private:
        std::shared_ptr<Camera2D> m_camera;
        SharedPtr<Kiwi::Editor::ImGuiSubsystem> m_imgui;

        std::vector<SpriteInstance> m_sprites;

        // Sprite images randomly handed out to quads (cat / dog / capybara). Quads that
        // get none stay flat-coloured squares.
        std::vector<SharedPtr<ATexture2D>> m_textures;

        f32 m_worldW = 0.0f;
        f32 m_worldH = 0.0f;
        f32 m_time   = 0.0f;

        // sweep state
        std::array<BenchResult, kCounts.size()> m_results{};
        i32  m_phaseIndex  = 0;
        i32  m_warmupLeft  = 0;
        i32  m_measureLeft = 0;
        f64  m_accumMs     = 0.0;
        bool m_running     = false;
        bool m_finished    = false;

        i32  m_currentCount = 2000;   // free-run count (when not sweeping)

        std::string m_gpu;
        std::string m_glVersion;
    };
}
