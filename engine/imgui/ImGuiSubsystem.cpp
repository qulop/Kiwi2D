#include "ImGuiSubsystem.hpp"
#include "ImGuiBackend.hpp"

#include <core/ProjectSubsystem.hpp>
#include <core/Project.hpp>

#include <misc/WindowSubsystem.hpp>

#include <backends/imgui_impl_glfw.h>



namespace Kiwi {
    bool ImGuiSubsystem::Init() {
        if (!Super::Init()) {
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        SetDarkThemeColors();
        m_backend = CreateImGuiBackend();

        std::shared_ptr<WindowSubsystem> windowSubsystem = GetSubsystem<WindowSubsystem>();
        KIWI_ENSURE(windowSubsystem);

        std::shared_ptr<AWindow> mainWindow = windowSubsystem->GetMainWindow();
        KIWI_ENSURE(mainWindow);


        m_backend->Init(mainWindow);

        const I32Rect windowSizes = mainWindow->GetWindowSizes();
        io.DisplaySize = ImVec2(
            static_cast<float>(windowSizes.Width()),
            static_cast<float>(windowSizes.Height())
        );

        return true;
    }

    void ImGuiSubsystem::DeInit() {
        Super::DeInit();

        m_backend->DeInit();
        ImGui::DestroyContext();
    }

    void ImGuiSubsystem::BeginFrame() const {
        m_backend->BeginFrame();
        ImGui::NewFrame();
    }

    void ImGuiSubsystem::EndFrame() const {
        ImGui::Render();
        m_backend->EndFrame();
    }

    void ImGuiSubsystem::SetDarkThemeColors() {
        ImGui::StyleColorsDark();
    }

    std::unique_ptr<IImGuiBackend> ImGuiSubsystem::CreateImGuiBackend() {
        std::shared_ptr<ProjectSubsystem> projectSubsystem = GetSubsystem<ProjectSubsystem>();
        KIWI_ENSURE(projectSubsystem);

        std::shared_ptr<Project> project = projectSubsystem->GetActiveProject();
        KIWI_ENSURE(project);

        switch (project->GetConfig().renderAPI) {
        case ERenderAPI::OpenGL:
            return std::make_unique<ImGuiBackendOpenGL>();
        case ERenderAPI::Vulkan:
            return std::make_unique<ImGuiBackendVulkan>();
        default:
            std::unreachable();
        }
    }
}
