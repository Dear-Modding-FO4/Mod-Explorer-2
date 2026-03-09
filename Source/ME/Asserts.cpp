#include <ME/Asserts.h>
#include <ME/Util.h>
#include <Windows.h>

#include <cstdlib>
#include <cstdio>
#include <format>

namespace ME
{
    static constexpr auto SIZE_ASSERT_BUFFERS = 4 * 1024;
    static char AdAssertBuffer[SIZE_ASSERT_BUFFERS]{};

    static int ReportAssertion([[maybe_unused]] const char* SourceFile, [[maybe_unused]] int SourceLine,
        [[maybe_unused]] const char* Function, const char* Message) noexcept
    {
        return MessageBoxA(nullptr, Message, "Assertion", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TOPMOST);
    }

    void AssertMsg(const char* SourceFile, int SourceLine, const char* Function, const char* FormattedMessage, ...) noexcept
    {
        std::fill_n(AdAssertBuffer, 1024, 0);

        va_list ap;
        va_start(ap, FormattedMessage);
        vsnprintf(AdAssertBuffer, SIZE_ASSERT_BUFFERS, FormattedMessage, ap);
        va_end(ap);

        std::string sAssertMessage = std::format("{}({}) {}():\n\n{}"sv, SourceFile, SourceLine, Function, AdAssertBuffer);

        while (true)
        {
            const auto sdl_assert_state = ReportAssertion(SourceFile, SourceLine, Function, sAssertMessage.c_str());

            if (sdl_assert_state == IDRETRY)
                // Fuck user
                continue;
            else if (sdl_assert_state == IDABORT)
            {
                // For debugger
                __debugbreak();
                // For Wine
                abort();
                // AGAIN!!!
                TerminateProcess(GetCurrentProcess(), 1);
                // CTD
                *((int*)nullptr) = 0;
            }

            // Skip
            break;
        }
    }
}