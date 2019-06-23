# OpenGoldSrc AC96/Evol (Ancient Code '96)

>Because Valve still updates the GoldSrc engine code sometimes (suddenly), we're counting from the birth date of the engine which is 1996 (not from the initial release in 1998)

## Architecture Notes

Notes for old architecture
  
The original code base of GoldSrc is mostly coming from the QuakeWorld back in 1996 (NetQuake -> QW above that -> Q2 above that -> Custom additions above that)  
This code has grown and lotta new hacks from Valve and later from ReHLDS team were added  
So maintainability of this code is really low  
The main value of this old code is that it's the same code that's currently present inside the original GoldSrc  
This fact raises the value of this old code from '96 for some people (for me personally) but it's really hard to work with it practically because it's just the shit code as it is  
So the only option here is to make a major refactoring anyway or just continue to write the shit code but sooner or later the code base will become absolutely unmaintainable and nobody would want to touch it (like in the original GS case)  
It's not about perfectionism, it's more about time-saving (save the old code and time now and lose it later N times bigger vs consuming some time now to rewrite it and save a lot of time later from that)  
I'm still thinking that this thing isn't worth to be fully rewritten but we'll see  
This engine clone isn't oriented on bugwards-compatibility with the original GoldSrc. That means that some of the old Valve's solution about the engine architecture won't be applied. There will be some compatibility for "features, not bugs" such as shield-bone #20 on the player model skeleton collision detection (yep, CS detects shield collision that way). But there won't be support for internal vgui for the engine code and there won't be any renderer code inside the engine (mostly, we still need some software renderer functionality for swds and such)  
I think that each engine subsystem will be compiled as a dll which will be dynamically linked with the engine dll (the Source way). But there still will be possible to link them statically inside the engine if you need to do that  

### Notable Differences
These changes differentiate the OGS from the original GoldSrc; They're found to be convenient/useful for the code base:
* No direct VGUI linking to engine module;
* Renderer code in its own dll;
* (Engine subsystems split into their own dlls;)?