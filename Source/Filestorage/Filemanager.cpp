/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-9-19
    Notes:
        Simply stores and fetches blocks of data.
*/

#include <Configuration\All.h>
#include <Filestorage\Filemanager.h>
#include <unordered_map>
#include <thread>

namespace Filemanager
{
    // The files we have loaded into memory.
    std::unordered_map<uint64_t, Filebody> Storage_DRIVE0;
    std::unordered_map<uint64_t, Filebody> Storage_DRIVE1;
    std::unordered_map<uint64_t, Filebody> Storage_DRIVE2;
    std::unordered_map<uint64_t, Filebody> Storage_DRIVE3;
    std::unordered_map<uint64_t, Filebody> Storage_DRIVE4;
    std::unordered_map<uint64_t, Filebody> Storage_CACHE;
    std::unordered_map<uint64_t, Fileheader> Manifest;
    std::unordered_map<uint8_t, Filelocation> Drive;
    std::string Rootdir;

    // Insert the filename/data into the list.
    void Loaddirectory(std::string Path, Filebandwidth Throttling)
    {
        std::vector<std::string> Filenames;

        // Replace ./ with root.
        if (std::strstr(Path.c_str(), "./"))
        {
            Path = Rootdir + (std::strstr(Path.c_str(), "./") + 2);
        }

        // Find all files in the dir.
        if (Filesystem::Searchdirrecursive(Path, &Filenames))
        {
            for each (auto File in Filenames)
            {
                Loadfile(File, Throttling);
            }
        }
    }
    void Loadfile(std::string Path, Filebandwidth Throttling)
    {
        // Replace ./ with root.
        if (std::strstr(Path.c_str(), "./"))
        {
            Path = Rootdir + (std::strstr(Path.c_str(), "./") + 2);
        }

        // Index the file if needed.
        auto Manifestentry = Manifest.find(FNV1a_Runtime_64(Path.c_str(), Path.length()));
        if (Manifestentry == Manifest.end())
        {
            Indexfile(Path);
            Manifestentry = Manifest.find(FNV1a_Runtime_64(Path.c_str(), Path.length()));
            if (Manifestentry == Manifest.end()) return;
        }

        // If already in memory, we assume it's properly loaded.
        if (Manifestentry->second.Inmemory) return;

        // Create a new fileentry from the manifest.
        Filebody Fileentry(Manifestentry->second);
        Fileentry.Data = std::make_unique<uint8_t[]>(Fileentry.Filesize);

        // Read the file into the buffer.
        if (!Throttling)
        {
            if (!Filesystem::Readfile(Fileentry.Filename.c_str(), Fileentry.Data.get(), &Fileentry.Filesize))
            {
                DebugPrint(va("Could not open file \"%s\"", Fileentry.Filename.c_str()));
                return;
            }
        }
        else
        {
            std::FILE *Filehande = std::fopen(Fileentry.Filename.c_str(), "rb");     
            if (!Filehande)
            {
                DebugPrint(va("Could not open file \"%s\"", Fileentry.Filename.c_str()));
                return;
            }

            for (size_t i = 0; i < (Fileentry.Filesize / Throttling) / 10; i += Throttling / 10)
            {
                auto Start = std::chrono::high_resolution_clock::now();

                std::fread(Fileentry.Data.get() + i, 1, Throttling / 10, Filehande);
                std::this_thread::sleep_until(Start + std::chrono::milliseconds(100));
            }

            std::fclose(Filehande);
        }

        // Insert the entry into the correct buffer.
        switch (Fileentry.Location)
        {
            case Filelocation::DRIVE0: Storage_DRIVE0[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Fileentry; break;
            case Filelocation::DRIVE1: Storage_DRIVE1[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Fileentry; break;
            case Filelocation::DRIVE2: Storage_DRIVE2[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Fileentry; break;
            case Filelocation::DRIVE3: Storage_DRIVE3[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Fileentry; break;
            case Filelocation::DRIVE4: Storage_DRIVE4[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Fileentry; break;
        }
    }
    void Indexdirectory(std::string Path)
    {
        std::vector<std::string> Filenames;

        // Set the root dir if we haven't.
        if (0 == Rootdir.size())
        {
            std::string Local = __argv[0];
            Rootdir = Local.substr(0, Local.find_last_of("/\\"));
        }

        // Replace ./ with root.
        if (std::strstr(Path.c_str(), "./"))
        {
            Path = Rootdir + (std::strstr(Path.c_str(), "./") + 2);
        }

        // Find all files in the dir.
        if (Filesystem::Searchdirrecursive(Path, &Filenames))
        {
            for each (auto File in Filenames)
            {
                Indexfile(File);
            }
        }
    }
    void Indexfile(std::string Path)
    {
         Fileheader Localfile;

          // Set the drive, will mess up for > 5.
         if (Drive.find(Path[0]) == Drive.end())
             if (Drive.size() < 5) Drive[Path[0]] = Filelocation(Path[0]);
             else DebugPrint("More than 5 drives used, this is not supported =(");
         Localfile.Location = Drive[Path[0]];

         // Set the identifying data.
         Localfile.Filename = Path;
         Localfile.Filesize = Filesystem::Filesize(Path.c_str());

         // Add to the collection.
         Manifest[FNV1a_Runtime_64(Path.c_str(), Path.length())] = Localfile;
    }
}
