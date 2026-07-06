#include "Editor.hpp"

#include <app/LauncherLayer.hpp>

#include <imgui/ImGuiSubsystem.hpp>

#include <misc/WindowSubsystem.hpp>

#include <core/Project.hpp>
#include <core/ProjectSubsystem.hpp>


namespace Kiwi::Editor {
    bool EditorApp::Init() {
        if (!Super::Init()) {
            return false;
        }

        RegisterSubsystem<ImGuiSubsystem>();
        if (!GetSubsystem<ImGuiSubsystem>()->Init()) {
            return false;
        }

        std::shared_ptr<ProjectSubsystem> projectSubsystem = GetSubsystem<ProjectSubsystem>();
        KIWI_ENSURE(projectSubsystem);

        if (!projectSubsystem->GetActiveProject()) {
            m_editorState = EEditorState::LAUNCHER;
            m_uiLayer = std::make_shared<LauncherLayer>();
        }
        else {
            m_editorState = EEditorState::EDITOR;
        }


        return m_uiLayer->Init();
    }

    void EditorApp::BeforeFrameBegin() {
        KIWI_ASSERT_BASIC(m_uiLayer);

        Super::BeforeFrameBegin();
        m_uiLayer->BeginFrame();
    }

    void EditorApp::BeforeFrameEnd() {
        KIWI_ASSERT_BASIC(m_uiLayer);

        Super::BeforeFrameEnd();
        m_uiLayer->EndFrame();
    }

    void EditorApp::Update() {
        KIWI_ASSERT_BASIC(m_uiLayer);

        Super::Update();
        m_uiLayer->Update();
    }
}

