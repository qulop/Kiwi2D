#include "PlatformVars.hpp"


namespace Kiwi::Globals::Platform {
    std::mutex g_consoleIOMutex = std::mutex();

    std::vector<String> g_cmdLineArguments = std::vector<String>();
}