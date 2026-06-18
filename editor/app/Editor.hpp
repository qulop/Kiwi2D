#pragma once

#include <core/Application.hpp>


namespace Kiwi::Editor {
    class ImGuiSubsystem;


    class Editor : public Application {
    public:
        using Super = Application;

    public:
        KIWI_NODISCARD bool Init() override;

        void BeforeFrameBegin() override;
        void BeforeFrameEnd() override;

    private:
        SharedPtr<ImGuiSubsystem> m_imGuiSubsystem;
    };
}