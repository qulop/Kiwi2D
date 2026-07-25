#include "SystemConsoleWin32.hpp"

#include <common/types/Opt.hpp>

#include <platform/PlatformVars.hpp>
#include "platform/Platform.hpp"


namespace {
    constexpr size_t READ_BUFFER_SIZE = 1024;
}


namespace Kiwi::Platform::Win32 {
    void SystemConsoleWin32::Write() {
        Write(StringView("\r\n", 2));
    }

    void SystemConsoleWin32::Write(StringView str) {
        KIWI_SCOPED_LOCK(Globals::Platform::g_consoleIOMutex);

        HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hnd == NULL || hnd == INVALID_HANDLE_VALUE) {
            return;
        }

        const char* data = str.data();
        DWORD remaining = static_cast<DWORD>(str.size());
        while (remaining > 0) {
            DWORD written = 0;
            if (!WriteFile(hnd, data, remaining, &written, nullptr) || written == 0) {
                break;
            }

            data += written;
            remaining -= written;
        }
    }

    void SystemConsoleWin32::WriteLine(StringView str) {
        String s(str.data(), str.size());
        s += "\r\n";
        Write(s.ToStringView());
    }

    void SystemConsoleWin32::Put(char ch) {
        Write(StringView(&ch, 1));
    }

    Opt<String> SystemConsoleWin32::ReadString() {
        KIWI_SCOPED_LOCK(Globals::Platform::g_consoleIOMutex);

        HANDLE hnd = GetStdHandle(STD_INPUT_HANDLE);
        if (hnd == NULL || hnd == INVALID_HANDLE_VALUE) {
            return ZERO_OPT;
        }

        char buffer[READ_BUFFER_SIZE];
        DWORD readCount = 0;

        if (!ReadConsoleA(hnd, buffer, READ_BUFFER_SIZE - 1, &readCount, nullptr)) {
            return ZERO_OPT;
        }

        buffer[readCount] = '\0';
        return String(&buffer[0], readCount);
    }

    Opt<i32> SystemConsoleWin32::ReadInt32() {
        return String::ParseIntI32(ReadString().GetOrDefault().ToStringView());
    }

    Opt<i64> SystemConsoleWin32::ReadInt64() {
        return String::ParseIntI64(ReadString().GetOrDefault().ToStringView());
    }

    Opt<float> SystemConsoleWin32::ReadFloat() {
        return String::ParseFloat(ReadString().GetOrDefault().ToStringView());
    }

    Opt<double> SystemConsoleWin32::ReadDouble() {
        return String::ParseDouble(ReadString().GetOrDefault().ToStringView());
    }

    void SystemConsoleWin32::SetCursorPosition(u16 x, u16 y) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {
            static_cast<SHORT>(x),
            static_cast<SHORT>(y)
        });
    }

    void SystemConsoleWin32::Clear() {
        KIWI_SCOPED_LOCK(Globals::Platform::g_consoleIOMutex);

        HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hnd == NULL || hnd == INVALID_HANDLE_VALUE) {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        DWORD count = 0;
        DWORD cellCount = 0;

        if (!GetConsoleScreenBufferInfo(hnd, &bufferInfo)) {
            return;
        }

        cellCount = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;

        FillConsoleOutputCharacter(hnd, ' ', cellCount, { 0, 0 }, &count);
        FillConsoleOutputAttribute(hnd, bufferInfo.wAttributes, cellCount, { 0, 0 }, &count);
        SetCursorPosition(0, 0);
    }

    void SystemConsoleWin32::Beep(i32 frequency, i32 duration) {
        ::Beep(frequency, duration);
    }

    void SystemConsoleWin32::Flush() {
        FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
    }

    void SystemConsoleWin32::FlushInput() {
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    }
}
