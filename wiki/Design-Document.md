# **O**pen **G**old**S**ource Engine
## Design document

![OGS Logo](https://github.com/Sh1ft0x0EF/OpenGoldSrc/blob/master/docs/OGSLogo1280x512.png?raw=true "OGS Logo")

***

### General:
**Oriented lang:** C++ (C++11 and higher)  

Oriented on cloning the original GoldSrc build 4554 for now and then add support for SDL2 (it will be applied after the first runnable engine build)

### Features:
* Wish-list https://github.com/ValveSoftware/halflife/issues/1712 will be implemented
* Optional MetaHook integration
* Windows mod DLLs loading on Linux
* Integrated subtitles system (from hl subsmod)
* New OOP API for game/client dlls (will be incompat with the old one)

### Ideas/Notes:
* Trinity Renderer integration
* x64 and multithreading support
* No-Steam build support
* IEditorAPI interface from the engine for in-game editor integration (which accepts a custom window handle) + additional editor mode for the engine itself
* In-game editor for level creation and testing (with ability to load uncompiled maps for testing/developing purposes)
* Support for HLSDK version 1 (at least for game dll) to be able to play old mods
* libcurl for resource downloading/uploading