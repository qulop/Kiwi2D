#pragma once

#include <core/Object.hpp>

#include <misc/Window.hpp>


namespace Kiwi {
    class WindowSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(WindowSubsystem, ASubsystem)

    public:
        bool Init() override;
        KIWI_NODISCARD bool CreateMainWindow(StringView title);

        KIWI_NODISCARD std::shared_ptr<AWindow> GetMainWindow();

        KIWI_NODISCARD std::vector<const char*> GetVulkanExtensionsForWindow(std::shared_ptr<AWindow> window);



    private:
        std::shared_ptr<AWindow> m_mainWindow;
    };
}