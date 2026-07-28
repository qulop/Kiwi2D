#include "GLFWWindow.hpp"

#include <core/LogSubsystem.hpp>
#include <core/input/InputSubsystem.hpp>
#include <core/input/KeyMapper.hpp>

#include <common/Definitions.hpp>
#include <common/types/CString.hpp>
#include <common/Debug.hpp>

#include <platform/Platform.hpp>

#include <renderer/Texture.hpp>


#ifdef KIWI_WIN32_USED
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Kiwi {
    bool GLFWWindow::Init(const WindowInitInfo& initInfo) {
        if (!Super::Init(initInfo)) {
            return false;
        }

	    if (!glfwInit()) {
	        KIWI_CTX_LOG(CRITICAL, "Failed to initialize the GLFW library");
	        return false;
	    }

        const Platform::DisplayInfo& displayInfo = initInfo.displayInfo;

	    glfwWindowHint(GLFW_RESIZABLE, initInfo.resizable ? GLFW_TRUE : GLFW_FALSE);

        m_renderAPI = initInfo.renderAPI;
        if (m_renderAPI == ERenderAPI::OpenGL) {
	        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        else {
	        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

	    GLFWmonitor* monitor = MapToGLFWmonitor(displayInfo);
        if (!monitor) {
            KIWI_CTX_LOG(ERROR, "Failed to map our DisplayInfo(with name {}) to GLFWmonitor*", displayInfo.name);
            return false;
        }

        i32 monitorX = 0, monitorY = 0;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);

	    m_window = glfwCreateWindow(
	        displayInfo.resolution.x, displayInfo.resolution.y,
	        initInfo.windowName.ToCString(), nullptr, nullptr
	    );
	    if (!m_window) {
	        KIWI_CTX_LOG(CRITICAL, "Failed to create the main window");
	        return false;
	    }

        if (m_renderAPI == ERenderAPI::OpenGL) {
            glfwMakeContextCurrent(m_window);
        }

        glfwSetWindowPos(m_window, monitorX, monitorY);

	    glfwSetWindowUserPointer(m_window, this);
	    SetupCallbacks();

        return true;
    }

    void GLFWWindow::ChangePosition(i32 x, i32 y) const {
        glfwSetWindowPos(m_window, x, y);
    }

    void GLFWWindow::Resize(i32 width, i32 height) {
        KIWI_ASSERT_BASIC(width > 0 && height > 0);
        KIWI_ASSERT(false, "Implement this");
    }

    void GLFWWindow::SetIcon(const String& path) {
        // TODO:
        // Texture texture{ path };
        // GLFWimage ico {
        //     .width = texture.GetWidth(),
        //     .height = texture.GetHeight(),
        //     .pixels = const_cast<u8*>(texture.GetData())
        // };
        // glfwSetWindowIcon(m_window, 1, &ico);

        KIWI_CTX_LOG(WARNING, "GLFWWindow::SetIcon is not implemented yet");
    }


    void GLFWWindow::SetTitle(const String& title) {
        glfwSetWindowTitle(m_window, title.ToCString());
    }


    void GLFWWindow::SetInnerCursor(const String& path) {
        // TODO:
        // Texture texture{ path };
        // GLFWimage cur {
        //     .width = texture.GetWidth(),
        //     .height = texture.GetHeight(),
        //     .pixels = const_cast<u8*>(texture.GetData())
        // };
        // m_cursor = glfwCreateCursor(&cur, 0, 0);
        // glfwSetCursor(m_window, m_cursor);

        KIWI_LOG(WARNING, "GLFWWindow::SetInnerCursor is not implemented yet");
    }


    void GLFWWindow::ResetInnerCursor() {
        glfwSetCursor(m_window, nullptr);
    }

    void GLFWWindow::SetWindowSize(U32Vec2 size) {
        glfwSetWindowSize(m_window, size.x, size.y);

        NotifyFramebufferResized(U32Rect(0, 0, size.x, size.y));
    }

    void GLFWWindow::SwapBuffers() {
        glfwSwapBuffers(m_window);
    }

    void GLFWWindow::PollEvents() {
        glfwPollEvents();
    }

    std::vector<const char*> GLFWWindow::GetVulkanExtensions() const {
        u32 extCount = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extCount);

        std::vector<const char*> res(extensions, extensions + extCount);
        return res;
    }

    KIWI_NODISCARD EWindowVendor GLFWWindow::GetVendor() const {
        return EWindowVendor::GLFW;
    }

    void* GLFWWindow::GetNativeWindowPtr() const {
        return m_window;
    }

    Platform::NativeWindowHandle GLFWWindow::GetNativeWindowHandle() const {
        if constexpr (GetCurrentPlatform() == ECurrentPlatform::WINDOWS) {
            return {
                .handle = glfwGetWin32Window(m_window)
            };
        }
        else {
            std::unreachable();
        }
    }

    ERenderAPI::Type GLFWWindow::GetRenderAPI() const {
        return m_renderAPI;
    }

    void GLFWWindow::MaximizeWindow(bool val) {
        m_isMaximized.store(val);
        if (m_isMaximized) {
            glfwMaximizeWindow(m_window);
        }
        else {
            glfwRestoreWindow(m_window);
        }
    }

    bool GLFWWindow::Update() {
        return Super::Update();
	}

    bool GLFWWindow::ShouldClose() const {
        return glfwWindowShouldClose(m_window) == GLFW_TRUE;
    }

    void GLFWWindow::SetVSyncEnable(bool val) {
       glfwSwapInterval(val ? 1 : 0);
    }

    KIWI_NODISCARD I32Rect GLFWWindow::GetWindowSizes() const {
        i32 width = 0, height = 0;
        glfwGetWindowSize(m_window, &width, &height);

        return I32Rect(0, 0, width, height);
    }

    KIWI_NODISCARD U32Rect GLFWWindow::GetFramebufferSizes() const {
        i32 width = 0, height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);

        return U32Rect(0u, 0u, width, height);
    }

	GLFWWindow::~GLFWWindow() {
        glfwDestroyWindow(m_window);
        glfwTerminate();

        if (m_cursor) {
            glfwDestroyCursor(m_cursor);
        }
	}

    GLFWmonitor* GLFWWindow::MapToGLFWmonitor(const Platform::DisplayInfo& display) const {
        i32 count = 0;
	    GLFWmonitor** monitors = glfwGetMonitors(&count);
	    for (i32 i = 0; i < count; ++i) {
            i32 x = 0, y = 0;
	        glfwGetMonitorPos(monitors[i], &x, &y);

	        if (display.displayPosition.x == x && display.displayPosition.y == y) {
	            return monitors[i];
	        }
	    }

	    return nullptr;
	}

    void GLFWWindow::SetupCallbacks() const {
        glfwSetErrorCallback([](int err, const char* desc) {
            if (const std::shared_ptr<LogSubsystem> logger = GetSubsystem<LogSubsystem>()) {
                logger->Log(ELogLevel::ERROR_LOG, "GLFW Error: the error code: {}, description: {}",
                    err, desc
                );
            }
        });

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            auto* self = static_cast<GLFWWindow*>(
                glfwGetWindowUserPointer(window)
            );

            if (self != nullptr) {
                self->NotifyFramebufferResized(U32Rect(0, 0, width, height));
            }
        });

		glfwSetKeyCallback(m_window, [](GLFWwindow*, int key, int scancode, int action, int mods) {
		    if (std::shared_ptr<InputSubsystem> inputSubsystem = GetSubsystem<InputSubsystem>()) {
		        const EKeyCode nativeKeyCode = KeyMapper::MapKeyGLFW(key);
		        const EKeyAction nativeAction = KeyMapper::MapActionGLFW(action);

		        inputSubsystem->UpdateKeyState(nativeKeyCode, nativeAction);
		    }
        });

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow*, int button, int action, int mods) {
		    if (std::shared_ptr<InputSubsystem> inputSubsystem = GetSubsystem<InputSubsystem>()) {
                const EKeyCode nativeMouseKeyCode = KeyMapper::MapKeyGLFW(button);
		        const EKeyAction nativeAction = KeyMapper::MapActionGLFW(action);

		        inputSubsystem->UpdateKeyState(nativeMouseKeyCode, nativeAction);
		    }
		});

        glfwSetCursorPosCallback(m_window, [](GLFWwindow*, double xpos, double ypos) {
            if (std::shared_ptr<InputSubsystem> inputSubsystem = GetSubsystem<InputSubsystem>()) {
                inputSubsystem->UpdateMousePosition(
                    Vec2(static_cast<f32>(xpos), static_cast<f32>(ypos))
                );
            }
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int isFocused)
        {
            auto* self = static_cast<GLFWWindow*>(
                glfwGetWindowUserPointer(window)
            );

            if (self != nullptr) {
                self->NotifyWindowFocusChanged(isFocused == GLFW_TRUE);
            }
        });

		// glfwSetScrollCallback(m_window, [](GLFWwindow* glfwWindow, double xpos, double ypos) {
		//     auto eventSubsystem = ((GLFWWindow*)glfwGetWindowUserPointer(glfwWindow))->GetSubsystem<EventSubsystem>();
  //
  //           eventSubsystem->Excite(MouseScrollEvent{ xpos, ypos });
  //       });
	}
}
