#pragma once

#include <core/Application.hpp>


namespace Kiwi {
    class ImGuiSubsystem;
    class AImGuiLayer;
}


namespace Kiwi::Editor {
    class EditorApp : public Application {
        KIWI_CREATE_OBJECT(EditorApp, Application)

    public:
        enum class EEditorState {
            NONE,
            LAUNCHER,
            EDITOR
        };

        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;
        void BeforeFrameEnd() override;

        void Update() override;


        ~EditorApp() override = default;

    private:
        EEditorState m_editorState = EEditorState::NONE;

        std::shared_ptr<AImGuiLayer> m_uiLayer;
    };
}