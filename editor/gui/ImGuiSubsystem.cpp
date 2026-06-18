#include "ImGuiSubsystem.hpp"

#include <misc/WindowSubsystem.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>



namespace {
    // Matches the OpenGL 4.6 Core context requested by GLFWWindow.
    constexpr const char* GLSL_VERSION = "#version 460";
}


namespace Kiwi::Editor {
    bool ImGuiSubsystem::Init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_contextCreated = true;

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        auto windowSubsystem = GetSubsystem<WindowSubsystem>();
        if (!windowSubsystem) {
            KIWI_CTX_LOG(ERROR, "ImGuiSubsystem requires the WindowSubsystem to be initialized first");
            return false;
        }

        auto mainWindow = windowSubsystem->GetMainWindow();
        if (!mainWindow) {
            KIWI_CTX_LOG(ERROR, "ImGuiSubsystem requires a created main window");
            return false;
        }

        const I32Rect sizes = mainWindow->GetWindowSizes();
        io.DisplaySize = ImVec2(
            static_cast<float>(sizes.bottomRight.x),
            static_cast<float>(sizes.bottomRight.y)
        );

        SetDarkThemeColors();

        // Attach the platform (GLFW) and renderer (OpenGL3) backends. The GL context
        // is already current (made current in GLFWWindow::Init), so this is safe here.
        auto* glfwWindow = static_cast<GLFWwindow*>(mainWindow->GetNativeWindowPtr());
        if (!glfwWindow) {
            KIWI_CTX_LOG(ERROR, "Failed to obtain the native GLFW window pointer for ImGui");
            return false;
        }

        if (!ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true)) {
            KIWI_CTX_LOG(ERROR, "ImGui_ImplGlfw_InitForOpenGL failed");
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init(GLSL_VERSION)) {
            KIWI_CTX_LOG(ERROR, "ImGui_ImplOpenGL3_Init failed");
            ImGui_ImplGlfw_Shutdown();
            return false;
        }

        m_backendsInitialized = true;
        return true;
    }

    void ImGuiSubsystem::DeInit() {
        if (m_backendsInitialized) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            m_backendsInitialized = false;
        }

        if (m_contextCreated) {
            ImGui::DestroyContext();
            m_contextCreated = false;
        }
    }

    void ImGuiSubsystem::BeginFrame() {
        if (!m_contextCreated) {
            return;
        }

        if (m_backendsInitialized) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
        }

        ImGui::NewFrame();
    }

    void ImGuiSubsystem::EndFrame() {
        if (!m_contextCreated) {
            return;
        }

        ImGui::Render();

        if (m_backendsInitialized) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        ImGui::EndFrame();
    }

    void ImGuiSubsystem::SetDarkThemeColors() {
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg]       = ImVec4(0.10f, 0.105f, 0.11f, 1.0f);

        colors[ImGuiCol_Header]         = ImVec4(0.20f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_HeaderHovered]  = ImVec4(0.30f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_HeaderActive]   = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

        colors[ImGuiCol_Button]         = ImVec4(0.20f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_ButtonHovered]  = ImVec4(0.30f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_ButtonActive]   = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

        colors[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

        colors[ImGuiCol_Tab]            = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_TabHovered]     = ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
        colors[ImGuiCol_TabActive]      = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
        colors[ImGuiCol_TabUnfocused]   = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.205f, 0.21f, 1.0f);

        colors[ImGuiCol_TitleBg]        = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_TitleBgActive]  = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    }
}