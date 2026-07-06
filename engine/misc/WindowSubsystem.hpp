#pragma once

#include <core/Object.hpp>

#include <misc/Window.hpp>


namespace Kiwi {
    class WindowSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(WindowSubsystem, ASubsystem)

    public:
        explicit WindowSubsystem(ERenderAPI::Type renderAPI);

        bool Init() override;
        KIWI_NODISCARD bool CreateMainWindow(const String& title);

        KIWI_NODISCARD std::shared_ptr<AWindow> GetMainWindow();

        KIWI_NODISCARD std::vector<const char*> GetVulkanExtensionsForWindow(std::shared_ptr<AWindow> window);

    private:
        ERenderAPI::Type m_renderAPI;
        std::shared_ptr<AWindow> m_mainWindow;
    };
}