/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-6-4
    Notes:
        Simple hooking classes to provide a starting point for modifications.
*/

#pragma once
#include <Configuration\All.h>
#include <functional>

#define EXTENDEDHOOKDECL(Basehook)                              \
template <typename Functionsignature, typename ...Arguments>    \
struct Basehook ##Ex : public Basehook                          \
{                                                               \
    std::function<Functionsignature> Originalfunction;          \
    void Setfunctionaddress(const void *Address = nullptr)      \
    {                                                           \
        if (!Address) Address = s_Location;                     \
        Originalfunction = *(Functionsignature *)Address;       \
    }                                                           \
}                                                               \

// Memory protection modifiers.
extern void Protectblock(const void *Address, const size_t Length, unsigned long Oldprotect);
extern unsigned long Unprotectblock(const void *Address, const size_t Length);

// The base interface for hooks.
struct IHook
{
    // Saved information about the placement.
    uint8_t s_Textdata[20];
    void *s_Location;
    void *s_Target;

    // Create and remove a hook at [location].
    virtual bool Installhook(const void *Location, const void *Target) = 0;
    virtual bool Removehook() = 0;
};

// A 'dumb' hook that just inserts a jump at the address.
struct Stomphook : public IHook
{
    // Create and remove a hook at [location].
    virtual bool Installhook(const void *Location, const void *Target) override;
    virtual bool Removehook() override;
};
EXTENDEDHOOKDECL(Stomphook);

// A 'dumb' hook that just inserts a call at the address.
struct Callhook : public IHook
{
    // Create and remove a hook at [location].
    virtual bool Installhook(const void *Location, const void *Target) override;
    virtual bool Removehook() override;
};
EXTENDEDHOOKDECL(Callhook);
