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
    public:
        ImGuiSubsystem() = default;

    public:
        KIWI_NODISCARD bool Init() override;

    };
}