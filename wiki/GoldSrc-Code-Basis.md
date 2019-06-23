>Half-Life's Code Basis  
>[Thu Aug 01, 2002 / 04:38pm PDT] Chris 'autolycus' Bokitch
>
>What came first, the chicken or the egg? What is Half-Life built on, Quake 1 or Quake 2? These questions pop up pretty frequently, and neither seems to have an accepted answer. In an effort to extinguish the argument, I've asked the people who know best. About Half-Life, that is. We're not touching the question about the chicken.
>
>Ken Birdwell explains it like this:
>
>"It is fundamentally just a heavily modified Quake 1 engine. There are about 50 lines of code from the Quake 2 engine, mostly bugs fixes to hard problems that Carmack found and fixed before we ran into them."
>
>At its core, it's a Quake 1 engine. You can tell this by comparing Half-life's map compiling tools with those shipped with Quake1. You'll find very minor differences -- none of them are fundamental. The core rendering is architecturally identical to Quake1, the only "significant" change is removing the fixed palette, making map lighting RGB instead of 8 bit, and converting software rendering to be 16 bit color instead of 8 bit color, which was pretty easy and only required minor code changes. Our skeletal animation system is new, though it was heavily influenced by the existing model rendering code, as were a lot of our updated particle effects, though less so with our beam system. Decals are totally new, our audio system has some major additions to what already existed, and at ship time our networking was almost totally Quake1 / QuakeWorld networking but about a year later Yahn rewrote most of all of it to be very different in design. The most highly changed sections are the game logic; ours being written in C++ and Quake's being in written interpreted "Quake C". Our AI system is very very different from anything in Quake, and there's a lot of other significant architectural changes in the whole server and client implementations, though if you look hard enough you can find a few remnants of some nearly unmodified Quake1 era entities buried in places.
>
>Jay Stelly adds, "We also took PAS from QW and/or Q2 and a couple of other minor routines I can remember (no more than 100-200 lines of code there). There was some feature overlap (as Ken mentions) like game code DLLs and colored lighting, but we developed our own solutions to those independent of Q2."
>So there it is. This should put some arguments to rest. Half-Life is based on Quake 1, although it has a very small amount of Quake 2 code. Yahn notes that "we did use some of the winsock functions from Q2, that's about it. Probably more than 50 lines, but nothing too interesting."

>Michael A. Hobson  
>Fri Jan 03, 2003 / 11:36am PST
>
>Based upon direct examination of the Half-Life hardware accellerated engine DLL ( HW.DLL ) from HL 1.1.0.7 and HL 1.1.0.8 with IDA Pro 4
>(Interactive Disassembler) and comparison with the Quake 1 and Quake 2 GPL'd source code releases, I can tell you for an absolute fact that
>Valve replaced all of the direct OpenGL calls used by glQuakeWorld with calls to the QGL wrapper API used in Quake 2. Valve then extended this API
>with a QGL Direct 3D wrapper and that is how they made the glQW renderer work with 3dFX miniGL, system OpenGL ( OPENGL32.DLL ) or D3D within a
>single engine DLL.
>
>The source code for the QGL API is in the file ...\quake2\win32\qgl_win.c and is well over 4000 lines -- hardly an insignificant piece.