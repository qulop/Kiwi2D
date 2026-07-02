#include <platform/Platform.hpp>
#include <platform/PlatformVars.hpp>

#include <common/cast/Cast.hpp>
#include <common/types/String.hpp>
#include <common/types/Opt.hpp>

#include <math/vec/Vec.hpp>

#include <shellapi.h>
#include <cstdio>


#ifdef KIWI_DEBUG_BUILD
    #define KIWI_HEAP_ALLOC_FLAGS HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY
#else
    #define KIWI_HEAP_ALLOC_FLAGS HEAP_ZERO_MEMORY
#endif


namespace {
    void BindStdStreamsToConsole() noexcept {
        const HANDLE hOut = CreateFileW(
            L"CONOUT$", GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, 
            OPEN_EXISTING, 0, nullptr
        );
        if (hOut != INVALID_HANDLE_VALUE) {
            SetStdHandle(STD_OUTPUT_HANDLE, hOut);
            SetStdHandle(STD_ERROR_HANDLE, hOut);
        }

        const HANDLE hIn = CreateFileW(
            L"CONIN$", GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, 
            OPEN_EXISTING, 0, nullptr
        );
        if (hIn != INVALID_HANDLE_VALUE) {
            SetStdHandle(STD_INPUT_HANDLE, hIn);
        }

        FILE* f = nullptr;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);

        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
}



namespace Kiwi::Platform {
    bool CreateDebugConsole() noexcept {
        const HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        const bool isStdOutAlreadyAttached = stdOut != nullptr && stdOut != INVALID_HANDLE_VALUE;
        if (isStdOutAlreadyAttached) {
            SetConsoleOutputCP(CP_UTF8);
            return true;
        }

        if (AttachConsole(ATTACH_PARENT_PROCESS) == TRUE) {
            BindStdStreamsToConsole();
            return true;
        }

        return CreateConsole();
    }

    bool CreateConsole() noexcept {
        if (AllocConsole() == FALSE) {
            return false;
        }

        BindStdStreamsToConsole();
        return true;
    }

    void DestroyConsole() noexcept {
        FreeConsole();
    }

    std::vector<DisplayInfo> EnumerateDisplays() noexcept {
        std::vector<DisplayInfo> result;

        auto&& callback = [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            auto* res = BasicCast::UnsafeCast<std::vector<DisplayInfo>*>(lParam);

            MONITORINFOEX info;
            info.cbSize = sizeof(info);
            if (!GetMonitorInfo(hMonitor, &info)) {
                return TRUE;
            }

            DisplayInfo props;
            props.nativeHandle = hMonitor;
            props.isPrimary = info.dwFlags & MONITORINFOF_PRIMARY;
            props.name = info.szDevice;
            props.displayPosition = U32Vec2(info.rcMonitor.left, info.rcMonitor.top);

            DEVMODE dm;
            dm.dmSize = sizeof(dm);
            if (EnumDisplaySettings(props.name.ToCString(), ENUM_CURRENT_SETTINGS, &dm)) {
                props.resolution = U32Vec2(dm.dmPelsWidth, dm.dmPelsHeight);
                props.refreshRate = dm.dmDisplayFrequency;
            }

            res->push_back(props);
            return TRUE;
        };


        EnumDisplayMonitors(nullptr, nullptr, callback, BasicCast::UnsafeCast<LPARAM>(&result));

        return result;
    }

    Opt<DisplayInfo> GetPrimaryDisplay() noexcept {
        std::vector<DisplayInfo> monitors = EnumerateDisplays();

        auto it = std::ranges::find_if(monitors, [](DisplayInfo& m) {
            return m.isPrimary;
        });

        return (it == monitors.end()) ? ZERO_OPT : Opt<DisplayInfo>{ *it };
    }

    size_t GetDisplaysCount() noexcept {
        return EnumerateDisplays().size();
    }

    bool IsDisplayCurrentlyActive(const DisplayInfo& info) noexcept {
        HWND hForegroundWindow = GetForegroundWindow();
        if (!hForegroundWindow) {
            return false;
        }

        HMONITOR hCurrentMonitor = MonitorFromWindow(hForegroundWindow, MONITOR_DEFAULTTONEAREST);
        if (!hCurrentMonitor) {
            return false;
        }

        HMONITOR hReceivedMonitor = BasicCast::UnsafeCast<HMONITOR>(info.nativeHandle);

        return hCurrentMonitor == hReceivedMonitor;
    }

    bool IsUnderDebug() noexcept {
        return IsDebuggerPresent();
    }

    std::vector<String> GetApplicationArguments() noexcept {
        if (!Globals::Platform::g_applicationArguments.empty()) {
            return Globals::Platform::g_applicationArguments;
        }

        i32 argc = 0;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        // Skip the executable path - we can obtain it by GetExecutablePath()
        for (i32 i = 1; i < argc; i++) {
            String arg = String::FromWideCharPtr(argv[i]);
            Globals::Platform::g_applicationArguments.emplace_back(std::move(arg));
        }

        return Globals::Platform::g_applicationArguments;
    }

    std::filesystem::path GetExecutablePath() noexcept {
        TCHAR path[MAX_PATH];
        GetModuleFileName(nullptr, path, MAX_PATH);
        KIWI_ASSERT(GetLastError() != ERROR_INSUFFICIENT_BUFFER, "Failed to get application path buffer: ERROR_INSUFFICIENT_BUFFER");

        return std::filesystem::path{ path };
    }

    std::filesystem::path GetExecutableDirectoryPath() noexcept {
        return GetExecutablePath().parent_path();
    }

    std::filesystem::path GetPathToSysTemp() noexcept {
        wchar_t path[MAX_PATH];
        GetTempPathW(MAX_PATH, path);

        return std::filesystem::path{ path };
    }

    String WideToUTF8(const wchar_t* wstr) noexcept {
        if (!wstr || CWideString::StrLen(wstr) == 0) {
            return String::EmptyString();
        }

        const i32 wideLen = static_cast<i32>(CWideString::StrLen(wstr));

        const i32 bytesNeeded = ::WideCharToMultiByte(
            CP_UTF8, 0, wstr, wideLen, nullptr, 0, nullptr, nullptr
        );
        if (bytesNeeded <= 0) {
            return String::EmptyString();
        }

        String utf8(static_cast<size_t>(bytesNeeded), '\0');
        
        const i32 written = ::WideCharToMultiByte(
            CP_UTF8, 0, wstr, wideLen, utf8.GetMutableRaw(), bytesNeeded, nullptr, nullptr
        );
        if (written <= 0) {
            return String::EmptyString();
        }

        return utf8;
    }


    void Breakpoint() {
        DebugBreak();
    }

    void CriticalShutdown() {
        TerminateProcess(GetCurrentProcess(), KIWI_EXIT_FAILURE);
    }
}


namespace Kiwi::Platform::Memory {
    void* NativeHeapAlloc(size_t sz) {
        HANDLE heap = GetProcessHeap();
        if (heap == nullptr) {
            return nullptr;
        }

        return HeapAlloc(heap, KIWI_HEAP_ALLOC_FLAGS, sz);
    }
    
    void NativeHeapFree(void* addr, KIWI_MAYBE_UNUSED size_t sz) {
        HANDLE heap = GetProcessHeap();
        if (heap == nullptr) {
            return;
        }

        HeapFree(heap, 0, addr);
    }

    void* CreateMemoryMapping() {
        return nullptr;
    }
}

#undef KIWI_HEAP_ALLOC_FLAGS