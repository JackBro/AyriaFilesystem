/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-6-4
    Notes:
        Inserts a dumb jump at the location.
*/

#include <Configuration\All.h>

// Virtual page permissions to avoid exceptions.
#ifdef _WIN32
#include <Windows.h>

void Protectblock(const void *Address, const size_t Length, unsigned long Oldprotect)
{
    unsigned long Temp;
    VirtualProtect((void *)Address, Length, Oldprotect, &Temp);
}
unsigned long Unprotectblock(const void *Address, const size_t Length)
{
    unsigned long Oldprotect;
    VirtualProtect((void *)Address, Length, PAGE_EXECUTE_READWRITE, &Oldprotect);
    return Oldprotect;
}
#else
#include <sys/mman.h>

void Protectblock(const void *Address, const size_t Length, unsigned long Oldprotect)
{
    mprotect(Address, Length, Oldprotect);
}
unsigned long Unprotectblock(const void *Address, const size_t Length)
{
    /*
        TODO(Convery):
        We need to parse /proc/self/maps to get the access.
        Implement this when needed.
    */

    mprotect(Address, Length, (PROT_READ | PROT_WRITE | PROT_EXEC));
    return (PROT_READ | PROT_EXEC);
}
#endif // _WIN32

// Create and remove a hook at [location].
#ifdef ENVIRONMENT64
bool Stomphook::Installhook(const void *Location, const void *Target)
{
    s_Location = (void *)Location;
    s_Target = (void *)Target;

    auto Protection = Unprotectblock(Location, 12);
    {
        // Save the text data before we overwrite it.
        std::memcpy(s_Textdata, Location, 12);

        // movabs rax, Target;
        // jmp rax;
        *(uint8_t *)(size_t(Location) + 0) = 0x48;
        *(uint8_t *)(size_t(Location) + 1) = 0xB8;
        *(uint64_t *)(size_t(Location) + 2) = uint64_t(Target);
        *(uint8_t *)(size_t(Location) + 10) = 0xFF;
        *(uint8_t *)(size_t(Location) + 11) = 0xE0;
    }
    Protectblock(Location, 12, Protection);

    return true;
}
bool Stomphook::Removehook()
{
    auto Protection = Unprotectblock(s_Location, 12);
    {
        // Restore the text data.
        std::memcpy(s_Location, s_Textdata, 12);
    }
    Protectblock(s_Location, 12, Protection);

    return true;
}
#else
bool Stomphook::Installhook(const void *Location, const void *Target)
{
    s_Location = (void *)Location;
    s_Target = (void *)Target;

    auto Protection = Unprotectblock(Location, 5);
    {
        // Save the text data before we overwrite it.
        std::memcpy(s_Textdata, Location, 5);

        *(uint8_t *)(size_t(Location) + 0) = 0xE8;
        *(uint32_t *)(size_t(Location) + 1) = uint32_t(Target);
    }
    Protectblock(Location, 5, Protection);

    return true;
}
bool Stomphook::Removehook()
{
    auto Protection = Unprotectblock(s_Location, 5);
    {
        // Restore the text data.
        std::memcpy(s_Location, s_Textdata, 5);
    }
    Protectblock(s_Location, 5, Protection);

    return true;
}
#endif
