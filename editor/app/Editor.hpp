#pragma once

#include <core/Application.hpp>


namespace Kiwi {
    class ImGuiSubsystem;
}


namespace Kiwi::Editor {
    class EditorApp : public Application {
        KIWI_CREATE_OBJECT(EditorApp, Application)

    public:
        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;
        void BeforeFrameEnd() override;

        void Update() override;


        ~EditorApp() override = default;

    private:
        std::shared_ptr<ImGuiSubsystem> m_imguiSubsystem;
    };
}