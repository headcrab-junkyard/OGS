I'm still planning to implement the C++ API for OGS and use in new format game/client dlls

I think some of the interfaces will be shared between the OGS and MGT (i.e. render dll interface)

At the same time, I don't want to use custom versions of interfaces that already brought by original GS (IVCVar, IFileSystem and such) in OGS so the ogsapi repo should contain only shared interfaces, usable by both engines

And about gamedll/clientdll interfaces - I think their interfaces would be shared between OGS and MGT too
I also want to modify the game code repo the way it will abstract the game code from the direct engine API usage so it would be possible to implement the engine API for both engines and use in one game codebase

About porting old code to new interfaces - it could be possible to fully port it or partially via special case (old code calls gamedll functions via c struct of func ptrs, new api is using the virtual c++ interfaces but you are able to implement the interface as redirector to old api funcs)