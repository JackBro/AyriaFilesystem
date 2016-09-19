/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-9-19
    Notes:
        The structures and enums related to the file-blocks.
*/

#pragma once
#include <Configuration\All.h>
#include <memory>

// Used for manifests.
enum Filelocation
{
    DRIVE0,
    DRIVE1,
    DRIVE2,
    DRIVE3,
    DRIVE4,

    CACHE,
};

// Used for enumeration/searching.
struct Fileheader
{
    Filelocation Location;
    std::string Filename;
    size_t Filesize;
    bool Inmemory;

    Fileheader() { Inmemory = false; };
};

// Data stored in memory, extends the header.
struct Filebody : public Fileheader
{
    std::unique_ptr<uint8_t[]> Data;
    uint64_t Expirationdate{ uint64_t(-1) };

    Filebody() { Inmemory = true; };
    Filebody(const Fileheader &Parent) : Fileheader(Parent) { Inmemory = true; };
    Filebody &Filebody::operator = (Filebody &Arg)
    {
        Data.swap(Arg.Data);

        Location = Arg.Location;
        Filename = Arg.Filename;
        Filesize = Arg.Filesize;
        Inmemory = Arg.Inmemory;

        Expirationdate = Arg.Expirationdate;
        return *this;
    }
};

// Bandwidth throttling in KB/s.
enum Filebandwidth
{
    SNAIL       = 1024,
    SLOW        = 8192,
    MEDIUM      = 32768,
    FAST        = 65536,
    UNCAPPED    = 0,
};
