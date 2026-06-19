#pragma once

#include <common/ProgramOptions.hpp>

#include <renderer/Renderer.hpp>

#include <core/EngineConfig.hpp>
#include <core/Object.hpp>

#include <misc/Window.hpp>


namespace Kiwi {
    class Engine final : public AObject {
        KIWI_CREATE_OBJECT(Engine, AObject)

    public:
        static constexpr bool DEFAULT_VSYNC_ENABLED = false;

    public:
        KIWI_NODISCARD bool Init(const ProgramOptions& opts);

        KIWI_NODISCARD bool Update();

        KIWI_NODISCARD bool IsRunning() const;

        ~Engine() override = default;

    private:
        std::shared_ptr<AWindow>  m_window;
        std::shared_ptr<Renderer> m_renderer;

        EngineConfig m_engineConfig;

        std::atomic<bool> m_isRunning = true;
    };
}
