#pragma once

#include <common/ProgramOptions.hpp>
#include <common/Definitions.hpp>

#include <common/Version.hpp>


namespace Kiwi::CmdLine {
    constexpr StringView VSYNC_ENABLE = "VSyncEnable";
    constexpr StringView FULL_SCREEN = "FullScreen";
    constexpr StringView MAX_FPS = "MaxFPS";
    constexpr StringView ENGINE_OUT_DIR = "EngineOutDir";
    constexpr StringView APPLICATION_OUT_DIR = "ApplicationOutDir";
    constexpr StringView WINDOW_NAME = "MainWindowName";
    constexpr StringView LOG_NAME = "LogFileName";
}


namespace Kiwi {
    struct EngineConfig {
    public:
        static constexpr Version ENGINE_VERSION = Version(1, 0, 0);
        static constexpr StringView ENGINE_NAME = "Kiwi Engine";

    #ifdef KIWI_DEBUG_BUILD
        static constexpr bool ENABLE_DEBUG_CAPABILITIES = true;
    #else
        static constexpr bool ENABLE_DEBUG_CAPABILITIES = false;
    #endif

    public:
        KIWI_NODISCARD static Vector<CmdLineOption> GetCommandLineOptions();
    };
}