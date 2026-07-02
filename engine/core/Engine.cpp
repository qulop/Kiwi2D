#include "Engine.hpp"

#include <core/EngineConfig.hpp>
#include <core/ProjectSubsystem.hpp>

#include <misc/WindowSubsystem.hpp>

#include <platform/window/GLFWWindow.hpp>

#include <profiler/EngineProfiler.hpp>


namespace Kiwi {
    bool Engine::Init(const ProgramOptions& opts) {
        // Project subsystem initialization
        Opt<i32> activeProjectID = opts
            .Get<i32>(CmdLine::ACTIVE_PROJECT_ID);

        RegisterSubsystem<ProjectSubsystem>(activeProjectID);
        if (!GetSubsystem<ProjectSubsystem>()->Init()) {
            return false;
        }

        // VSync enabled/disabled
        m_engineConfig.vsyncEnabled = opts
            .Get<bool>(CmdLine::VSYNC_ENABLE)
            .ValueOr(m_engineConfig.vsyncEnabled);  // By default disabled

        // Window creation
        std::shared_ptr<WindowSubsystem> windowSubsystem = GetSubsystem<WindowSubsystem>();

        String wndTitle = opts
            .Get<String>(CmdLine::WINDOW_NAME)
            .ValueOr(EngineConfig::ENGINE_NAME);
        if (!windowSubsystem->CreateMainWindow(wndTitle.ToStringView())) {
            return false;
        }

        m_window = windowSubsystem->GetMainWindow();
        m_window->SetVSyncEnable(m_engineConfig.vsyncEnabled);
        m_window->MaximizeWindow(true);

        // Renderer initalization
        m_renderer = std::make_shared<Renderer>();
        if (!m_renderer->Init()) {
            return false;
        }

        m_renderer->SetViewport(m_window->GetWindowSizes());

        return true;
    }

    bool Engine::Update() {
        KIWI_PROFILE_ZONE_NAME("Engine::Update");

        return m_window->Update();
    }

    bool Engine::IsRunning() const {
        return m_isRunning.load();
    }
}
