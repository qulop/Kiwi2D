#pragma once

#include <imgui/ImGuiLayer.hpp>


namespace Kiwi::Editor {
    class LauncherLayer : public AImGuiLayer {
        KIWI_CREATE_OBJECT(LauncherLayer, AImGuiLayer)

    public:
        void Update() override;

        ~LauncherLayer() override = default;
    };
}