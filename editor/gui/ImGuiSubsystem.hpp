#pragma once

#include <core/Object.hpp>

#include <renderer/pipeline/RenderAPI.hpp>

#include <common/Errors.hpp>



namespace Kiwi {
    class AWindow;
}

namespace Kiwi::Editor {
    template<ERenderAPI::Type API>
    struct InitImGuiForAPI;


    class ImGuiSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(ImGuiSubsystem, ASubsystem);

    public:
        ImGuiSubsystem() = default;
        ~ImGuiSubsystem() override = default;

    public:
        KIWI_NODISCARD bool Init() override;
        void DeInit() override;

        void BeginFrame();
        void EndFrame();

    private:
        void SetDarkThemeColors();

    private:
        bool m_contextCreated = false;
        bool m_backendsInitialized = false;
    };
}