/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-10-12
    Notes:
        Changes filepaths for fopen, nothing fancy.
*/

#include <STDInclude.h>
#include <unordered_map>

std::unordered_map<std::wstring /* Old */, std::wstring /* New */> Replacementmap;
std::unordered_map<std::string, void *> Stomphooks;

#ifdef _WIN32
#include <Windows.h>

HANDLE __stdcall ReplaceCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    HANDLE Result;
    std::string Ascii(lpFileName);

    auto Replace = Replacementmap.find({ Ascii.begin(), Ascii.end() });
    if (Replace != Replacementmap.end())
    {
        Ascii = { Replace->second.begin(), Replace->second.end() };
        lpFileName = Ascii.c_str();
    }

    auto _Pointer = Stomphooks[__FUNCTION__];
    if (_Pointer)
    {
        auto _Function = ((COAL::Hook::StomphookEx<decltype(CreateFileA)> *)_Pointer)->Originalfunction;
        ((COAL::Hook::StomphookEx<decltype(CreateFileA)> *)_Pointer)->Removehook();
        Result = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        ((COAL::Hook::StomphookEx<decltype(CreateFileA)> *)_Pointer)->Reinstall();
    }
    else
    {
        Result = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }

    return Result;
}
HANDLE __stdcall ReplaceCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    HANDLE Result;

    auto Replace = Replacementmap.find(lpFileName);
    if (Replace != Replacementmap.end())
        lpFileName = Replace->second.c_str();

    auto _Pointer = Stomphooks[__FUNCTION__];
    if (_Pointer)
    {
        auto _Function = ((COAL::Hook::StomphookEx<decltype(CreateFileW)> *)_Pointer)->Originalfunction;
        ((COAL::Hook::StomphookEx<decltype(CreateFileW)> *)_Pointer)->Removehook();
        Result = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        ((COAL::Hook::StomphookEx<decltype(CreateFileW)> *)_Pointer)->Reinstall();
    }
    else
    {
        Result = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }

    return Result;
}
#else

#endif

// Add an entry to the map from another plugin.
extern "C" EXPORT_ATTR void __cdecl Replacefile(const char *Oldname, const char *Newname)
{
    std::string Index = Oldname;
    std::string Value = Newname;

    Replacementmap[{Index.begin(), Index.end()}] = { Value.begin(), Value.end() };
}

// Initialize the system.
void InitializeFopenReplacement()
{
    size_t Address;

#ifdef _WIN32

    #define PATCH_FS_STOMP(Module, Export, Function)                                                                    \
    Address = (size_t)GetProcAddress(GetModuleHandleA(Module), Export);                                                 \
    if (Address)                                                                                                        \
    {                                                                                                                   \
        Stomphooks[#Function] = new COAL::Hook::StomphookEx<decltype(Function)>();                                      \
        ((COAL::Hook::StomphookEx<decltype(Function)> *)Stomphooks[#Function])->Setfunctionaddress((void *)Address);    \
        ((COAL::Hook::StomphookEx<decltype(Function)> *)Stomphooks[#Function])->Installhook((void *)Address, Function); \
    }                                                                                                                   \

    // Deep hooks, will catch other plugins as well.
    if (std::strstr(GetCommandLineA(), "-fs_stomphook"))
    {
        PATCH_FS_STOMP("Kernel32.dll", "CreateFileA", ReplaceCreateFileA);
        PATCH_FS_STOMP("Kernel32.dll", "CreateFileW", ReplaceCreateFileW);
    }
    else
    {
        Address = COAL::IAT::Findfunction("Kernel32.dll", "CreateFileA");
        if (Address) *(size_t *)Address = size_t(ReplaceCreateFileA);
        Address = COAL::IAT::Findfunction("Kernel32.dll", "CreateFileW");
        if (Address) *(size_t *)Address = size_t(ReplaceCreateFileW);
    }
    
#else

#endif

    // Load replacements from a file if available.
    COAL::CSV::Manager CSVReader;
    if (CSVReader.Readfile("./Plugins/AyriaFS/Replacements.csv"))
    {
        for (size_t Row = 0; ; ++Row)
        {
            // End of buffer check.
            if (CSVReader.Value(Row, 0).size() == 0)
                break;

            Replacefile(CSVReader.Value(Row, 0).c_str(), CSVReader.Value(Row, 1).c_str());
        }
    }
}
