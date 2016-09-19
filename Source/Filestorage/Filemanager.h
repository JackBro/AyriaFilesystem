/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-9-19
    Notes:
        Simply stores and fetches blocks of data.
*/

#pragma once
#include <Configuration\All.h>
#include <Filestorage\Fileheader.h>

namespace Filemanager
{
    // Insert the filename/data into the list.
    void Loaddirectory(std::string Path, Filebandwidth Throttling = Filebandwidth::UNCAPPED);
    void Loadfile(std::string Path, Filebandwidth Throttling = Filebandwidth::UNCAPPED);
    void Indexdirectory(std::string Path);
    void Indexfile(std::string Path);
}
