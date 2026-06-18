#include "Editor.hpp"

#include <gui/ImGuiSubsystem.hpp>

#include <misc/WindowSubsystem.hpp>



namespace Kiwi::Editor {
    bool Editor::Init() {
        if (!Super::Init()) {
            return false;
        }

        RegisterSubsystem<ImGuiSubsystem>();
        m_imGuiSubsystem = GetSubsystem<ImGuiSubsystem>();
        if (!m_imGuiSubsystem->Init()) {
            return false;
        }

        return true;
    }

    void Editor::BeforeFrameBegin() {
        if (m_imGuiSubsystem) {
            m_imGuiSubsystem->BeginFrame();
        }
    }

    void Editor::BeforeFrameEnd() {
        if (m_imGuiSubsystem) {
            m_imGuiSubsystem->EndFrame();
        }
    }
}

