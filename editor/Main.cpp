#ifdef KIWI_RELEASE_BUILD
    #define _NDEBUG
#endif

#include <core/EntryPoint.hpp>
#include <core/Application.hpp>

#include <common/PCH.hpp>

#include <platform/Platform.hpp>

#include <common/sync/Thread.hpp>

#include <app/Editor.hpp>


namespace Kiwi {
    i32 EntryPoint(i32 argc, char** argv) {
        using namespace Kiwi;

        Sync::Thread::MakeMainThead();


        i32 exitCode = KIWI_EXIT_SUCCESS;
        {
            std::unique_ptr<Application> app = std::make_unique<Editor::EditorApp>();
            if (!app->Init()) {
                return KIWI_EXIT_FAILURE;
            }

            exitCode = app->Run();
        }

        return exitCode;
    }
}

#ifdef KIWI_WIN32_USED
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    #ifdef KIWI_MSVC_USED
        return Kiwi::EntryPoint(__argc, __argv);
    #else
        std::vector<Kiwi::String> args = Kiwi::Platform::GetCmdLineArguments();
        std::vector<char*> argv = args
            | std::views::transform([](Kiwi::String& s) -> char* { return s.GetMutableRaw(); })
            | std::ranges::to<std::vector<char*>>();

        return Kiwi::EntryPoint(static_cast<Kiwi::i32>(argv.size()), argv.data());
    #endif
    }
#endif