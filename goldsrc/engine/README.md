# Engine module

# Overview

This is the engine module. It's using the original WinQuake/NetQuake codebase as a base and some of the QuakeWorld, Quake 2, and Quake 3 improvements and fixes applied above that (so it went a similar way of evolution to the original GoldSrc engine).

It was also heavily modified. The VM/progs support code was replaced with direct DLL loading and interaction to make it support the game module using the same interface as GS provides, which was manually rewritten using Quake 2's interface as a base. Some of the client-side code was moved over to the client DLL module. The support for the GameUI module was also added and the original legacy menu code was adapted to be an implementation of this module. The file system code was also moved to the filesystem module. The code was also modified to be buildable into a separate DLL loadable by either the launcher application or dedicated launcher application both created by reusing the original sources of the engine.

In order to reimplement some of the pieces of the original engine its binaries were analyzed for internal/static strings which provides a lot of useful clues regarding what and where supposed to be in the reimplemented engine. Those strings aren't exposing any specific details regarding the implementation or the actual engine code so technically that's not a reverse-engineering and they can be freely used for engine reimplementation.