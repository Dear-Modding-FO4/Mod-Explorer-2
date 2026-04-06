#include <Windows.h>

HMODULE globalDllHandle = { nullptr };

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        globalDllHandle = hModule;
        break;
    case DLL_PROCESS_DETACH:
        globalDllHandle = nullptr;
        break;
    default:
        break;
    }
    return TRUE;
}

