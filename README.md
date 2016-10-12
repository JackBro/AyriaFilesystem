Ayrias Filesystem Plugin
---

This plugin is intended to speed up loading of an applications files. The initial version is planned to feature:

- [x] Replacement of fopen to allow plugins to load alternative gamefiles.
- [x] Read a list of replacements from `./Plugins/AyriaFS/Replacements.csv`
- [ ] RAM-drive implementation for faster loading of select directories.
- [ ] Heatmap over the gamefiles to pre-load common files.
- [ ] RAID0-ish fileloading based on the fopen redirection.
- [ ] Loading files based on triggers, e.g. when `q3dm1.bsp` loads; also load `/models/players/mychar.skin`

Extensionloading
--
While you can inject your new extension into an application and have it run, the easiest way would be to use the Bootstrap module from Ayria (https://github.com/AyriaPublic/NativeBootstrap).
Once injected, this module will attempt to load all extensions from ./Plugins/ and call the initialization exports as needed.
As such, if you are creating an extension for an existing system, simply drop it into your ./Plugins/ directory.
