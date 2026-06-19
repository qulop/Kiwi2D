#include "Editor.hpp"

#include <imgui/ImGuiSubsystem.hpp>

#include <misc/WindowSubsystem.hpp>



namespace Kiwi::Editor {
    bool EditorApp::Init() {
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

    void EditorApp::BeforeFrameBegin() {
        Super::BeforeFrameBegin();

        m_imguiSubsystem->BeginFrame();
    }

    void EditorApp::BeforeFrameEnd() {
        Super::BeforeFrameEnd();

        m_imguiSubsystem->EndFrame();
    }

    void EditorApp::Update() {
        Super::Update();

        if (ImGui::Begin("Hierarchy")) {
            ImGui::Text("Main Camera");
        }
        ImGui::End();
    }
}

