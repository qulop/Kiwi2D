#include "Editor.hpp"

#include <imgui/ImGuiSubsystem.hpp>

#include <misc/WindowSubsystem.hpp>



namespace Kiwi::Editor {
    bool Editor::Init() {
        if (!Super::Init()) {
            return false;
        }

        RegisterSubsystem<ImGuiSubsystem>();
        m_imguiSubsystem = GetSubsystem<ImGuiSubsystem>();
        KIWI_ENSURE(m_imguiSubsystem);

        if (!m_imguiSubsystem->Init()) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize ImGui");
            return false;
        }

        return true;
    }

    void Editor::BeforeFrameBegin() {
        Super::BeforeFrameBegin();

        m_imguiSubsystem->BeginFrame();
    }

    void Editor::BeforeFrameEnd() {
        Super::BeforeFrameEnd();

        m_imguiSubsystem->EndFrame();
    }

    void Editor::Update() {
        Super::Update();

        if (ImGui::Begin("Hierarchy")) {
            ImGui::Text("Main Camera");
        }
        ImGui::End();
    }
}

