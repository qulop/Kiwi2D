#pragma once

#include <core/Object.hpp>
#include <common/Definitions.hpp>
#include <imgui/ImGuiBackend.hpp>

#include <imgui.h>


namespace Kiwi {
    class AWindow;

    class ImGuiSubsystem : public ASubsystem {
        KIWI_CREATE_OBJECT(ImGuiSubsystem, ASubsystem);

    public:
        ImGuiSubsystem() = default;

    public:
        KIWI_NODISCARD bool Init() override;
        void DeInit() override;

        void BeginFrame() const;
        void EndFrame() const;


    private:
        void SetDarkThemeColors();

        std::unique_ptr<IImGuiBackend> CreateImGuiBackend();

    private:
        std::unique_ptr<IImGuiBackend> m_backend;
    };
}