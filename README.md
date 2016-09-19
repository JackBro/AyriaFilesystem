Ayrias Filesystem Plugin
---

This plugin is intended to speed up loading of an applications files. The initial version is planned to feature:

* A RAM-drive implementation to load everything from a directory into memory.
* Loading of related files into memory based on filename-triggers.
* Multi-drive directories, basically a RAID0 implementation to maximize bandwidth usage.

Extensionloading
--
While you can inject your new extension into an application and have it run, the easiest way would be to use the Bootstrap module from Ayria (https://github.com/AyriaPublic/NativeBootstrap).
Once injected, this module will attempt to load all extensions from ./Plugins/ and call the initialization exports as needed.
As such, if you are creating an extension for an existing system, simply drop it into your ./Plugins/ directory.
