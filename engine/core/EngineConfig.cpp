#include "EngineConfig.hpp"

#include <common/ProgramOptions.hpp>



namespace Kiwi {
    std::vector<CmdLineOption> EngineConfig::GetCommandLineOptions() {
        return std::vector {
            CmdLineOption(CmdLine::VSYNC_ENABLE, EOptionArgType::BOOL),
            CmdLineOption(CmdLine::FULL_SCREEN, EOptionArgType::BOOL),
            CmdLineOption(CmdLine::MAX_FPS, EOptionArgType::INT),
            CmdLineOption(CmdLine::ENGINE_OUT_DIR, EOptionArgType::PATH),
            CmdLineOption(CmdLine::APPLICATION_OUT_DIR, EOptionArgType::PATH),
            CmdLineOption(CmdLine::WINDOW_NAME, EOptionArgType::STRING),
            CmdLineOption(CmdLine::LOG_NAME, EOptionArgType::STRING)
        };
    }

}
