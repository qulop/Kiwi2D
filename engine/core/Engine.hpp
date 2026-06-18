#pragma once

#include <common/ProgramOptions.hpp>

#include <renderer/Renderer.hpp>

#include <core/EngineConfig.hpp>
#include <core/Object.hpp>

#include <misc/Window.hpp>

#include <sync/Atomic.hpp>


namespace Kiwi {
    class Engine final : public AObject {
        KIWI_CREATE_OBJECT(Engine, AObject)

    public:
        KIWI_NODISCARD bool Init(const ProgramOptions& opts);

        KIWI_NODISCARD bool Update();

        KIWI_NODISCARD bool IsRunning() const;

        KIWI_NODISCARD SharedPtr<Renderer> GetRenderer() const { return m_renderer; }
        KIWI_NODISCARD SharedPtr<AWindow> GetWindow() const { return m_window; }

        ~Engine() override = default;

    private:
        std::shared_ptr<AWindow>  m_window;
        std::shared_ptr<Renderer> m_renderer;

        EngineConfig m_engineConfig;

        Atomic<bool> m_isRunning = true;

        bool m_vsyncEnable = true;
        u16 m_fpsLimit = 120;
    };
}
