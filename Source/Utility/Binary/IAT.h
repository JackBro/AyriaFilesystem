/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-10-12
    Notes:
        Fetches information from the binaries IAT.
*/

#pragma once
#include <cstdint>
#include <string>

namespace COAL
{
    namespace IAT
    {
        size_t Findfunction(std::string Module, std::string Function);
        size_t Findordinal(std::string Module, uint32_t Ordinal);
    }
}
