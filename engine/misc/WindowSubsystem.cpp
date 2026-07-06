#include "WindowSubsystem.hpp"


namespace Kiwi {
    WindowSubsystem::WindowSubsystem(ERenderAPI::Type renderAPI) :
        m_renderAPI(renderAPI)
    {}

    bool WindowSubsystem::Init() {
        Super::Init();

        return true;
    }

    bool WindowSubsystem::CreateMainWindow(const String& title) {
        KIWI_ASSERT(!m_mainWindow, "Main window already created!");

        std::vector<Platform::DisplayInfo> displayInfos = Platform::EnumerateDisplays();
        if (displayInfos.empty()) {
            KIWI_CTX_LOG(ERROR, "Failed to enumerate system displays");
            return false;
        }

        auto selectedDisplay = std::ranges::find_if(displayInfos, [](const auto& displayInfo) {
            return Platform::IsDisplayCurrentlyActive(displayInfo);
        });
        if (selectedDisplay == displayInfos.end()) {
            KIWI_CTX_LOG(WARNING, "Failed to find a preferred display to use. We will use the first one found earlier");
            selectedDisplay = displayInfos.begin();
        }

        KIWI_CTX_LOG(INFO, "Selected display: {} - {} with resolution {}x{} and refresh rate {}Hz",
            selectedDisplay->model,
            selectedDisplay->name,
            selectedDisplay->resolution.x,
            selectedDisplay->resolution.y,
            selectedDisplay->refreshRate
        );

        WindowInitInfo wndInitInfo;
        wndInitInfo.windowName = title;
        wndInitInfo.displayInfo = *selectedDisplay;
        wndInitInfo.renderAPI = m_renderAPI;
        wndInitInfo.resizable = false;  // TODO

        m_mainWindow = AWindow::CreateWindowImpl();
        if (!m_mainWindow->Init(wndInitInfo)) {
            return false;
        }

        return true;
    }

    std::shared_ptr<AWindow> WindowSubsystem::GetMainWindow() {
        KIWI_ENSURE(m_mainWindow);

        return m_mainWindow;
    }

    std::vector<const char*> WindowSubsystem::GetVulkanExtensionsForWindow(std::shared_ptr<AWindow> window) {
        KIWI_ENSURE(window);

        return window->GetVulkanExtensions();
    }
}
