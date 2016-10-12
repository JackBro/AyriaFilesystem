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
        return CreateFileA(Result->second.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    else
        return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
HANDLE __stdcall ReplaceCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    std::wstring Wide(lpFileName);
    std::string Ascii(Wide.begin(), Wide.end());

    auto Result = Replacementmap.find(Ascii);
    if (Result != Replacementmap.end())
        return CreateFileA(Result->second.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    else
        return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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
#ifdef _WIN32
    auto Address = COAL::IAT::Findfunction("Kernel32.dll", "CreateFileA");
    if (Address) *(size_t *)Address = size_t(ReplaceCreateFileA);
    Address = COAL::IAT::Findfunction("Kernel32.dll", "CreateFileW");
    if (Address) *(size_t *)Address = size_t(ReplaceCreateFileW);
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
