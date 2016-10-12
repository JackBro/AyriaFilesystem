/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-10-12
    Notes:
        Entrypoint for the plugin.
*/

#include <STDInclude.h>
#include <cstdint>
#include <cstdarg>

// Systems we manage.
extern void InitializeFopenReplacement();

extern "C"
{
    // Ayrias extension exports as per the 2016 standard.
    EXPORT_ATTR void __cdecl onExtensionUnloading(void)
    {
    }
    EXPORT_ATTR void __cdecl onInitializationStart(void)
    {
        InitializeFopenReplacement();
    }
    EXPORT_ATTR void __cdecl onInitializationComplete(void)
    {
    }
    EXPORT_ATTR void __cdecl onMessage(uint32_t Message, ...)
    {
        std::va_list Variadic;
        va_start(Variadic, Message);

        // Messages are a 32bit FNV1a hash of a string.
        switch (Message)
        {

        case COAL::FNV1::Compiletime::FNV1a_32("DefaultCase"):
        default:
            break;
        }

        va_end(Variadic);
    }
}

#ifdef _WIN32
#include <Windows.h>
BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    switch ( nReason )
    {
    case DLL_PROCESS_ATTACH:
        // Rather not handle all thread updates.
        DisableThreadLibraryCalls( hDllHandle );

        // Clean the logfile so we only save this session.
        COAL::Debug::DeleteLogfile();
        break;

    case DLL_THREAD_ATTACH:
        break;
    }

    return TRUE;
}

#else

void __attribute__((constructor)) SOMain()
{
    // Clean the logfile so we only save this session.
    DeleteLogfile();
}

#endif
