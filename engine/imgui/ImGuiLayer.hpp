#pragma once

#include <core/Object.hpp>


namespace Kiwi {
    class ImGuiSubsystem;


    class AImGuiLayer : public AObject {
        KIWI_CREATE_OBJECT(AImGuiLayer, AObject)

    public:
        virtual bool Init();

        virtual void BeginFrame();
        virtual void EndFrame();

        virtual void Update() = 0;


        ~AImGuiLayer() override = default;

    private:
        std::shared_ptr<ImGuiSubsystem> m_imguiSubsystem;
    };
}