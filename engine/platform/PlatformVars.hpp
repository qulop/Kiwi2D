#pragma once

#include <common/meta/TypeTraits.hpp>
#include <common/types/String.hpp>


namespace Kiwi::Globals::Platform {
    extern std::mutex g_consoleIOMutex;

    // Note: Call Platform::GetApplicationArguments() instead of directly use this variable
    extern Vector<String> g_applicationArguments;
}