#include "ImGuiBackend.hpp"

#include <misc/Window.hpp>


#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>



namespace Kiwi {
    void ImGuiBackendOpenGL::Init(std::shared_ptr<AWindow> mainWindow) {
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)mainWindow->GetNativeWindowPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 460 core");
    }

    void ImGuiBackendOpenGL::DeInit() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

    void ImGuiBackendOpenGL::BeginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    }

    void ImGuiBackendOpenGL::EndFrame() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }



    void ImGuiBackendVulkan::Init(std::shared_ptr<AWindow> mainWindow) {
        KIWI_NOT_IMPLEMENTED();
    }

    void ImGuiBackendVulkan::DeInit() {
        KIWI_NOT_IMPLEMENTED();
    }

    void ImGuiBackendVulkan::BeginFrame() {
        KIWI_NOT_IMPLEMENTED();
    }

    void ImGuiBackendVulkan::EndFrame() {
        KIWI_NOT_IMPLEMENTED();
    }
}
