#include "ImGuiLayer.hpp"

#include <imgui/ImGuiSubsystem.hpp>


namespace Kiwi {
    bool AImGuiLayer::Init() {
        m_imguiSubsystem = GetSubsystem<ImGuiSubsystem>();

        return m_imguiSubsystem != nullptr;
    }

    void AImGuiLayer::BeginFrame() {
        KIWI_ASSERT_BASIC(m_imguiSubsystem);

        m_imguiSubsystem->BeginFrame();
    }

    void AImGuiLayer::EndFrame() {
        KIWI_ASSERT_BASIC(m_imguiSubsystem);

        m_imguiSubsystem->EndFrame();
    }
}
