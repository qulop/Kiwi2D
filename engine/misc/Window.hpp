#pragma once

#include <core/Object.hpp>

#include <platform/Platform.hpp>

#include <renderer/pipeline/RenderAPI.hpp>

#include <math/Rect.hpp>


namespace Kiwi {
    enum class EWindowVendor {
        UNKNOWN,
        GLFW,
        WIN32_NATIVE,
        X11
    };


    struct WindowInitInfo {
        String windowName;
        Platform::DisplayInfo displayInfo;
        ERenderAPI::Type renderAPI;
        bool resizable = false;
    };


    class AWindow : public AObject {
        KIWI_CREATE_OBJECT(AWindow, AObject);

    public:
        using PFN_FramebufferResizeCallback = std::function<void(U32Rect)>;
        using PFN_WindowFocusChangedCallback = std::function<void(bool)>;


        KIWI_NODISCARD static std::shared_ptr<AWindow> CreateWindowImpl();

    public:
        KIWI_NODISCARD virtual bool Init(const WindowInitInfo& initInfo);

        virtual void ChangePosition(i32 x, i32 y) const = 0;
        virtual void Resize(i32 width, i32 height) = 0;
        virtual void ToCenter() const;
        virtual void SetIcon(const String& path) = 0;
        virtual void SetTitle(const String& title) = 0;
        virtual void SetInnerCursor(const String& path) = 0;
        virtual void ResetInnerCursor() = 0;
        virtual void SetWindowSize(U32Vec2 size) = 0;

        virtual void SwapBuffers() = 0;
        virtual void PollEvents() = 0;

        KIWI_NODISCARD virtual std::vector<const char*> GetVulkanExtensions() const = 0;

        KIWI_NODISCARD virtual EWindowVendor GetVendor() const = 0;
        KIWI_NODISCARD virtual void* GetNativeWindowPtr() const = 0;
        KIWI_NODISCARD virtual Platform::NativeWindowHandle GetNativeWindowHandle() const = 0;

        KIWI_NODISCARD virtual ERenderAPI::Type GetRenderAPI() const = 0;

        virtual void MaximizeWindow(bool val) = 0;

        KIWI_NODISCARD virtual bool IsMaximized() const;

        KIWI_NODISCARD virtual bool Update();
        KIWI_NODISCARD virtual bool ShouldClose() const = 0;

        virtual void SetVSyncEnable(bool val) = 0;

        virtual void AddFramebufferResizeCallback(const PFN_FramebufferResizeCallback& callback);
        virtual void AddWindowFocusChangedCallback(const PFN_WindowFocusChangedCallback& callback);

        KIWI_NODISCARD virtual I32Rect GetWindowSizes() const = 0;
        KIWI_NODISCARD virtual U32Rect GetFramebufferSizes() const = 0;

        KIWI_NODISCARD virtual size_t GetWidth() const;
        KIWI_NODISCARD virtual u32 GetFramebufferWidth() const;

        KIWI_NODISCARD virtual size_t GetHeight() const;
        KIWI_NODISCARD virtual u32 GetFramebufferHeight() const;

        ~AWindow() override = default;

    protected:
		void NotifyFramebufferResized(U32Rect newSizes);
        void NotifyWindowFocusChanged(bool isFocused);

        template<typename TCallback, typename... Args>
        void NotifyWindowEvent(const std::vector<TCallback>& callbacks, Args&&... args) {
            for (const auto& callback : callbacks) {
                std::invoke(callback, std::forward<Args>(args)...);
            }
        }

    protected:
        ERenderAPI::Type m_renderAPI = ERenderAPI::NONE;

        std::vector<PFN_FramebufferResizeCallback> m_framebufferResizeCallbacks;
        std::vector<PFN_WindowFocusChangedCallback> m_windowFocusChangedCallbacks;

        std::atomic<bool> m_isMaximized = false;
        std::atomic<bool> m_isResizable = false;
    };
}