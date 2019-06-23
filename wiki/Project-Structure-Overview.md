### Dependencies:
* hlsdk-core
* goldsrc-src
* (vgui)
* bzip2
* cppunitlite
* sdl2
* ogsapi

### Modules:
***(AC96 code base)***
* common - should be compiled in static lib and used by multiple other libs
* tier1 - static, used by multiple other libs (mostly because of interface.cpp)
* cl_dll - client dll implementations
* dlls - server dll implementations
* dedicated - dedicated engine launcher (hlds)
* dedicated_legacy - very old dedicated engine launcher, should be merged and removed
* engine - main engine code; should be called engine since we have no sw/hw-specific builds because we split the render module
* hltv - all hltv related stuff;
* launcher - main engine launcher (hl)
* render - all rendering code; should work the same as in Q2 (hot-swapping support)
* tier0 - various common code
* vstdlib - Q_ funcs and various other utils
* filesystem - fs module
* gameui - user interface module
* vgui
* vgui2
