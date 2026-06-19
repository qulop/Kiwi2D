#include "PlatformVars.hpp"


namespace Kiwi::Globals::Platform {
    std::mutex g_consoleIOMutex = std::mutex();

    Vector<String> g_applicationArguments = Vector<String>();
}