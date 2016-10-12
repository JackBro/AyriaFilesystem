/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-10-12
    Notes:
        Changes filepaths for fopen, nothing fancy.
*/

#include <STDInclude.h>
#include <unordered_map>

std::unordered_map<std::string /* Old */, std::string /* New */> Replacementmap;

#ifdef _WIN32
#include <Windows.h>

HANDLE __stdcall ReplaceCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    auto Result = Replacementmap.find(lpFileName);
    if (Result != Replacementmap.end())
        lpFileName = Result->second.c_str();

    return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

#else

#endif

// Add an entry to the map from another plugin.
extern "C" EXPORT_ATTR void __cdecl Replacefile(const char *Oldname, const char *Newname)
{
    Replacementmap[Oldname] = Newname;
}

// Initialize the system.
void InitializeFopenReplacement()
{
    auto Address = COAL::IAT::Findfunction("Kernel32.dll", "CreateFileA");
    if (Address) *(size_t *)Address = size_t(ReplaceCreateFileA);
}
