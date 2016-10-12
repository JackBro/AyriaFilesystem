/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-10-12
    Notes:
        Reads and writes to CSV files.
*/

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace COAL
{
    namespace CSV
    {
        class Manager
        {
            std::vector<std::vector<std::string>> Buffer;

        public:
            bool Readfile(std::string Filepath);
            bool Writefile(std::string Filepath);

            std::string Value(size_t Row, size_t Col);
            void Insert(std::vector<std::string> Line);
        };

        std::vector<std::vector<std::string>> Readfile(std::string Filepath);
        bool Writefile(std::vector<std::vector<std::string>> Buffer, std::string Filepath);
    }
}
