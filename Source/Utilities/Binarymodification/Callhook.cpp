/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-6-4
    Notes:
        Inserts a dumb call at the location.
*/

#include <Configuration\All.h>

// Create and remove a hook at [location].
#ifdef ENVIRONMENT64
bool Callhook::Installhook(const void *Location, const void *Target)
{
    s_Location = (void *)Location;
    s_Target = (void *)Target;

    auto Protection = Unprotectblock(Location, 12);
    {
        // Save the text data before we overwrite it.
        std::memcpy(s_Textdata, Location, 12);

        // movabs rax, Target;
        // call rax;
        *(uint8_t *)(size_t(Location) + 0) = 0x48;
        *(uint8_t *)(size_t(Location) + 1) = 0xB8;
        *(uint64_t *)(size_t(Location) + 2) = uint64_t(Target);
        *(uint8_t *)(size_t(Location) + 10) = 0xFF;
        *(uint8_t *)(size_t(Location) + 11) = 0xD0;
    }
    Protectblock(Location, 12, Protection);

    return true;
}
bool Callhook::Removehook()
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
bool Callhook::Installhook(const void *Location, const void *Target)
{
    s_Location = (void *)Location;
    s_Target = (void *)Target;

    auto Protection = Unprotectblock(Location, 5);
    {
        // Save the text data before we overwrite it.
        std::memcpy(s_Textdata, Location, 5);

        *(uint8_t *)(size_t(Location) + 0) = 0xE9;
        *(uint32_t *)(size_t(Location) + 1) = uint32_t(Target);
    }
    Protectblock(Location, 5, Protection);

    return true;
}
bool Callhook::Removehook()
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