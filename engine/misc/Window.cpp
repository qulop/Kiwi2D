#include "Window.hpp"

#include <platform/window/GLFWWindow.hpp>
#include <platform/Platform.hpp>

#include <profiler/EngineProfiler.hpp>


namespace Kiwi {
    std::shared_ptr<AWindow> AWindow::CreateWindowImpl() {
        // At the moment we have only implementation for GLFW 🥲
        if constexpr (GetCurrentPlatform() == ECurrentPlatform::WINDOWS) {
            return std::make_shared<GLFWWindow>();
        }

        std::unreachable();
    }

    bool AWindow::Init(const WindowInitInfo& initInfo) {
	    KIWI_ASSERT(
	        initInfo.displayInfo.resolution.x && initInfo.displayInfo.resolution.y,
	        "Width and/or height cannot be least or equal zero!"
	    );

        return true;
    }

    void AWindow::ToCenter() const {
        if (auto primaryDisplay = Platform::GetPrimaryDisplay()) {
            const I32Vec2 centerVec = GetWindowSizes().Center();

            const i32 cx = (CastTo<i32>(primaryDisplay->resolution.x) / 2) - centerVec.x;
            const i32 cy = (CastTo<i32>(primaryDisplay->resolution.y) / 2) - centerVec.y;

            ChangePosition(cx, cy);
        }
        else {
            KIWI_CTX_LOG(ERROR, "Failed to get window size");
        }
    }

    bool AWindow::Update() {
        KIWI_PROFILE_ZONE;

        PollEvents();
        SwapBuffers();

        return !IsWindowClosed();
    }

    bool AWindow::IsMaximized() const {
        return m_isMaximized.load();
    }

    size_t AWindow::GetWidth() const {
        return GetWindowSizes().Width();
    }

    u32 AWindow::GetFramebufferWidth() const {
        return GetFramebufferSizes().Width();
    }

    size_t AWindow::GetHeight() const {
        return GetWindowSizes().Height();
    }

    u32 AWindow::GetFramebufferHeight() const {
        return GetFramebufferSizes().Height();
    }
}
