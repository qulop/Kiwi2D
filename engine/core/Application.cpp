#include "Application.hpp"

#include <common/Time.hpp>

#include <profiler/EngineProfiler.hpp>

#include <core/Object.hpp>
#include <core/EngineConfig.hpp>
#include <core/LogSubsystem.hpp>
#include <core/ProjectSubsystem.hpp>

#include <platform/io/SystemConsole.hpp>

#include <misc/WindowSubsystem.hpp>



namespace Kiwi {
    std::filesystem::path Application::s_applicationOutputDirectory = std::filesystem::path{};


    std::filesystem::path Application::GetApplicationOutputDirectory() {
        KIWI_ASSERT_BASIC(!s_applicationOutputDirectory.empty());

        return s_applicationOutputDirectory;
    }

    bool Application::Init() {
    #ifdef KIWI_DEBUG_BUILD
        if (!Platform::CreateDebugConsole()) {
            return false;
        }
    #endif

        s_subsystems = KIWI_NOTHROW_NEW AObject::SubsystemHolder();
        if (!s_subsystems) {
            Console::WriteLine("Failed to allocate memory for the subsystems holder");
            return false;
        }

        if (EngineProfiler::IsEnabled() && !EngineProfiler::IsConnectedToServer()) {
            Console::WriteLine("Connection to the profiler server failed to establish");
        }

        if (Opt options = ProgramOptions::Parse(Platform::GetApplicationArguments()); options) {
           m_cliOptions = std::move(*options);
        }
        else {
            return false;
        }

        s_applicationOutputDirectory = m_cliOptions
            .Get<std::filesystem::path>(CmdLine::APPLICATION_OUT_DIR)
            .value_or(Platform::GetPathToSysTemp());

        // Logger initialization
        LoggerInitInfo loggerInfo {
            .loggerPathDirectory = GetApplicationOutputDirectory()
        };
        RegisterSubsystem<LogSubsystem>(loggerInfo);
        if (!GetSubsystem<LogSubsystem>()->Init()) {
            return false;
        }

        // Window initialization
        RegisterSubsystem<WindowSubsystem>();
        GetSubsystem<WindowSubsystem>()->Init();


        m_engine = std::make_shared<Engine>();
        if (!m_engine->Init(m_cliOptions)) {
            KIWI_CTX_LOG(ERROR, "Failed to initialize the engine instance");
            return false;
        }

        m_isInitialized.store(true);
        return true;
    }

    void Application::BeforeShutdown() {
        ShutdownAllSubsystems();
        delete s_subsystems;

        Console::WriteLine("All subsystems deleted");
    }

    i32 Application::Run() {
        KIWI_ASSERT(m_isInitialized.load(), "You must initialize an application first");

        BeforeRun();

        while (true) {
            BeforeFrameBegin();

            this->Update();

            if (!m_engine->Update()) {
                break;
            }

            BeforeFrameEnd();
        }

        BeforeShutdown();

        return KIWI_EXIT_SUCCESS;
    }
}
