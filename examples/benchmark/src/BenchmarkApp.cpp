#include "BenchmarkApp.hpp"

#include <core/Engine.hpp>

#include <renderer/Renderer.hpp>
#include <renderer/Camera2D.hpp>

#include <input/InputSubsystem.hpp>
#include <input/KeyCodes.hpp>

#include <gui/ImGuiSubsystem.hpp>

#include <common/Time.hpp>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <algorithm>
#include <cmath>


namespace Benchmark {
    namespace {
        // Builds a translate * rotate * scale matrix for a unit quad centred on the
        // origin. Each sprite is animated (rotation + pulse), which is the realistic
        // per-quad transform cost for a 2D engine and the load the test profiles.
        glm::mat4 MakeTransform(const Vec2& center, const Vec2& size, f32 angle) {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0.0f));
            m = glm::rotate(m, angle, glm::vec3(0.0f, 0.0f, 1.0f));
            return glm::scale(m, glm::vec3(size.x, size.y, 1.0f));
        }
    }


    bool BenchmarkApp::Init() {
        if (!Super::Init()) {
            return false;
        }

        // VSync must be off: the test measures raw renderer throughput, not the
        // display refresh rate. Without this the FPS would be capped to the monitor.
        auto window = GetEngine()->GetWindow();
        window->SetVSyncEnable(false);

        m_worldW = static_cast<f32>(window->GetFramebufferWidth());
        m_worldH = static_cast<f32>(window->GetFramebufferHeight());
        if (m_worldW < 1.0f) m_worldW = 1280.0f;
        if (m_worldH < 1.0f) m_worldH = 720.0f;

        m_camera = std::make_shared<Camera2D>(0.0f, m_worldW, 0.0f, m_worldH);

        RegisterSubsystem<Editor::ImGuiSubsystem>();
        m_imgui = GetSubsystem<Editor::ImGuiSubsystem>();
        if (!m_imgui->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the ImGui subsystem");
            return false;
        }

        // Real device / driver strings — glad is already loaded by the render context.
        if (const GLubyte* r = glGetString(GL_RENDERER)) {
            m_gpu = reinterpret_cast<const char*>(r);
        }
        if (const GLubyte* v = glGetString(GL_VERSION)) {
            m_glVersion = reinterpret_cast<const char*>(v);
        }

        RebuildSprites(m_currentCount);
        return true;
    }

    void BenchmarkApp::RebuildSprites(i32 count) {
        count = std::max(1, count);
        m_sprites.clear();
        m_sprites.reserve(static_cast<size_t>(count));

        // Lay the quads out as a screen-filling grid (aspect-aware), so every test
        // count covers the whole viewport instead of a corner.
        const f32 aspect = m_worldW / m_worldH;
        const i32 cols = std::max(1, static_cast<i32>(std::ceil(std::sqrt(static_cast<f32>(count) * aspect))));
        const i32 rows = std::max(1, static_cast<i32>(std::ceil(static_cast<f32>(count) / static_cast<f32>(cols))));

        const f32 cellW = m_worldW / static_cast<f32>(cols);
        const f32 cellH = m_worldH / static_cast<f32>(rows);
        const Vec2 size{ cellW * 0.82f, cellH * 0.82f };

        for (i32 i = 0; i < count; ++i) {
            const i32 cx = i % cols;
            const i32 cy = i / cols;

            SpriteInstance s;
            s.basePos = Vec2{ (static_cast<f32>(cx) + 0.5f) * cellW, (static_cast<f32>(cy) + 0.5f) * cellH };
            s.size = size;

            const f32 h = static_cast<f32>(i) * 0.35f;   // rainbow spread over the field
            s.color = Vec4{ 0.5f + 0.5f * std::sin(h),
                            0.5f + 0.5f * std::sin(h + 2.094f),
                            0.5f + 0.5f * std::sin(h + 4.188f),
                            1.0f };
            s.phase = static_cast<f32>(i % 97) * 0.0647f;

            m_sprites.push_back(s);
        }

        m_currentCount = count;
    }

    i32 BenchmarkApp::ComputeDrawCalls(i32 spriteCount) {
        // The batch renderer flushes one DrawIndexed per MaxQuads accumulated; colour
        // quads share texture slot 0, so the only splits come from the quad limit.
        const i32 maxQuads = static_cast<i32>(Kiwi::Renderer::MaxQuads);
        return (spriteCount + maxQuads - 1) / maxQuads;
    }

    void BenchmarkApp::StartSweep() {
        m_results = {};
        m_phaseIndex = 0;
        m_running = true;
        m_finished = false;
        m_accumMs = 0.0;
        m_warmupLeft = kWarmupFrames;
        m_measureLeft = kMeasureFrames;
        RebuildSprites(kCounts[0]);
        KIWI_CTX_LOG(INFO, "Benchmark sweep started");
    }

    void BenchmarkApp::AdvanceSweep(f32 dt) {
        const f32 frameMs = dt * 1000.0f;

        if (m_warmupLeft > 0) {
            --m_warmupLeft;
            return;
        }
        if (m_measureLeft <= 0) {
            return;
        }

        m_accumMs += static_cast<f64>(frameMs);
        --m_measureLeft;
        if (m_measureLeft > 0) {
            return;
        }

        // Window finished — record the averaged result for this sprite count.
        const f32 avgMs = static_cast<f32>(m_accumMs / static_cast<f64>(kMeasureFrames));
        BenchResult& r = m_results[static_cast<size_t>(m_phaseIndex)];
        r.sprites = kCounts[static_cast<size_t>(m_phaseIndex)];
        r.drawCalls = ComputeDrawCalls(r.sprites);
        r.frameMs = avgMs;
        r.fps = (avgMs > 0.0f) ? (1000.0f / avgMs) : 0.0f;
        r.valid = true;
        KIWI_CTX_LOG(INFO, "  {} sprites: {} draw call(s), {} ms, {} FPS",
                     r.sprites, r.drawCalls, r.frameMs, r.fps);

        ++m_phaseIndex;
        if (m_phaseIndex >= static_cast<i32>(kCounts.size())) {
            m_running = false;
            m_finished = true;
            RebuildSprites(2000);   // calm field to leave on screen
            KIWI_CTX_LOG(INFO, "Benchmark sweep finished");
            return;
        }

        m_accumMs = 0.0;
        m_warmupLeft = kWarmupFrames;
        m_measureLeft = kMeasureFrames;
        RebuildSprites(kCounts[static_cast<size_t>(m_phaseIndex)]);
    }

    void BenchmarkApp::Update() {
        const f32 dt = Time::DeltaTime();
        m_time += dt;

        auto input = GetSubsystem<InputSubsystem>();
        if (input) {
            if (input->IsKeyPressed(Key::Escape)) {
                Stop();
            }
            if (!m_running) {
                if (input->IsKeyPressed(Key::Space)) {
                    StartSweep();
                }
                // Keys 1..7 pick a fixed sprite count for a static screenshot.
                for (i32 k = 0; k < static_cast<i32>(kCounts.size()); ++k) {
                    const i32 key = static_cast<i32>(Key::D1) + k;
                    if (input->IsKeyPressed(key) && m_currentCount != kCounts[static_cast<size_t>(k)]) {
                        RebuildSprites(kCounts[static_cast<size_t>(k)]);
                    }
                }
            }
        }

        if (m_running) {
            AdvanceSweep(dt);
        }

        RenderScene();
        RenderOverlay();
        if (m_imgui) {
            m_imgui->EndFrame();
        }
    }

    SharedPtr<Kiwi::Renderer> BenchmarkApp::Renderer() const {
        return GetEngine()->GetRenderer();
    }

    void BenchmarkApp::BeforeFrameBegin() {
        if (m_imgui) {
            m_imgui->BeginFrame();
        }
    }

    void BenchmarkApp::RenderScene() {
        auto renderer = Renderer();
        if (!renderer) {
            return;
        }

        renderer->BeginScene(*m_camera);
        for (const SpriteInstance& s : m_sprites) {
            const f32 pulse = 0.85f + 0.15f * std::sin(m_time * 2.0f + s.phase);
            renderer->SubmitDraw(
                MakeTransform(s.basePos, Vec2{ s.size.x * pulse, s.size.y * pulse }, m_time * 0.4f + s.phase),
                s.color);
        }
        renderer->EndScene();
    }

    void BenchmarkApp::RenderOverlay() {
        const ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f) {
            return;
        }

        ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.78f);
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("Kiwi2D - Renderer load test", nullptr, flags)) {
            ImGui::Text("Batch renderer stress test (VSync OFF)");
            if (!m_gpu.empty()) {
                ImGui::Text("GPU: %s", m_gpu.c_str());
            }
            if (!m_glVersion.empty()) {
                ImGui::Text("OpenGL: %s", m_glVersion.c_str());
            }
            ImGui::Separator();

            ImGui::Text("Live: %d sprites | %.0f FPS | %.2f ms",
                        m_currentCount, io.Framerate, (io.Framerate > 0.0f) ? 1000.0f / io.Framerate : 0.0f);

            if (m_running) {
                const i32 done = kMeasureFrames - std::max(0, m_measureLeft);
                ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.4f, 1.0f),
                                   "Measuring %d sprites...  frame %d/%d",
                                   kCounts[static_cast<size_t>(m_phaseIndex)], done, kMeasureFrames);
            }
            else if (m_finished) {
                ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.55f, 1.0f), "Sweep complete.");
            }
            else {
                ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.0f, 1.0f),
                                   "SPACE: run full sweep   |   1-7: set count   |   ESC: quit");
            }

            ImGui::Separator();

            const ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                           ImGuiTableFlags_SizingFixedFit;
            if (ImGui::BeginTable("results", 4, tflags)) {
                ImGui::TableSetupColumn("Sprites");
                ImGui::TableSetupColumn("Draw calls");
                ImGui::TableSetupColumn("Frame time, ms");
                ImGui::TableSetupColumn("FPS (avg)");
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < kCounts.size(); ++i) {
                    const BenchResult& r = m_results[i];
                    ImGui::TableNextRow();
                    if (m_running && static_cast<i32>(i) == m_phaseIndex) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(70, 70, 30, 180));
                    }

                    ImGui::TableSetColumnIndex(0); ImGui::Text("%d", kCounts[i]);
                    if (r.valid) {
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%d", r.drawCalls);
                        ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", r.frameMs);
                        ImGui::TableSetColumnIndex(3); ImGui::Text("%.0f", r.fps);
                    }
                    else {
                        ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("-");
                        ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("-");
                        ImGui::TableSetColumnIndex(3); ImGui::TextDisabled("-");
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
