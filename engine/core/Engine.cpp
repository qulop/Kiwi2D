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

        std::shared_ptr<ProjectSubsystem> projectSubsystem = GetSubsystem<ProjectSubsystem>();
        if (!projectSubsystem->Init()) {
            return false;
        }

        ERenderAPI::Type renderAPI = projectSubsystem->GetRenderAPI();

        // VSync enabled/disabled
        m_engineConfig.vsyncEnabled = opts
            .Get<bool>(CmdLine::VSYNC_ENABLE)
            .ValueOr(m_engineConfig.vsyncEnabled);  // Disabled by default

        // Window initialization and creation
        RegisterSubsystem<WindowSubsystem>(renderAPI);

        std::shared_ptr<WindowSubsystem> windowSubsystem = GetSubsystem<WindowSubsystem>();
        if (!windowSubsystem->Init()) {
            return false;
        }

        String wndTitle = opts
            .Get<String>(CmdLine::WINDOW_NAME)
            .ValueOr(EngineConfig::ENGINE_NAME);
        if (!windowSubsystem->CreateMainWindow(wndTitle.ToStringView())) {
            return false;
        }

        m_window = windowSubsystem->GetMainWindow();
        m_window->SetVSyncEnable(m_engineConfig.vsyncEnabled);
        m_window->MaximizeWindow(true);

        // Renderer initialization
        m_renderer = std::make_shared<Renderer>();
        if (!m_renderer->Init(renderAPI)) {
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
