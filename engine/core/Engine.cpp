#include "Engine.hpp"

#include <core/EngineConfig.hpp>

#include <misc/WindowSubsystem.hpp>

#include <platform/window/GLFWWindow.hpp>

#include <profiler/EngineProfiler.hpp>


namespace Kiwi {
    bool Engine::Init(const ProgramOptions& opts) {
        m_vsyncEnable = opts.Get<bool>(CmdLine::VSYNC_ENABLE).value_or(m_vsyncEnable);

        std::shared_ptr<WindowSubsystem> windowSubsystem = GetSubsystem<WindowSubsystem>();

        String wndTitle = opts.Get<String>(CmdLine::WINDOW_NAME).value_or(EngineConfig::ENGINE_NAME);
        if (!windowSubsystem->CreateMainWindow(wndTitle.ToStringView())) {
            return false;
        }

        m_window = windowSubsystem->GetMainWindow();
        m_window->SetVSyncEnable(m_vsyncEnable);
        m_window->MaximizeWindow(true);

        m_renderer = MakeShared<Renderer>();
        if (!m_renderer->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the renderer");
            return false;
        }

        m_renderer->SetViewport(m_window->GetWindowSizes());

        return true;
    }

    bool Engine::Update() {
        // KIWI_PROFILE_ZONE_NAME("Engine::Update");

        // Poll input + present the frame. The app layer is responsible for clearing
        // and drawing (Renderer::BeginScene clears) before this runs; see
        // Application::Run() which calls the app's frame hooks around this.
        return m_window->Update();
    }

    bool Engine::IsRunning() const {
        return m_isRunning.load();
    }
}
