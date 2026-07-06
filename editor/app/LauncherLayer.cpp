#include "LauncherLayer.hpp"

#include <platform/Platform.hpp>

#include <imgui.h>


namespace Kiwi::Editor {
    void LauncherLayer::Update() {
        ImGui::Begin("Test");
        if (ImGui::Button("Close")) {
            Platform::ExitProcess();
        }

        float f = 0.f;
        ImGui::SliderFloat("Check", &f, 0.0f, 10.0f);

        ImGui::End();
    }
}
