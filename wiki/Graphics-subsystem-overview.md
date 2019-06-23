# OGS Grapics subsystem

### Problems:
* Currently the graphics code is in really messy state
* Lot of duplications from NetQuake - QuakeWorld - Quake2 (need to merge all of them)

### Oriented struct and features:
* Quake2 rendering code as basis (Use ref_gl and ref_soft code)
* Quake2 render dll split (ability to freely modify the rendering code)
* Additions from CSBTE codebase to gl render (shaders, fbo, etc)
* New render interface using Valve's module factory
* New renederdll will be using the new engine OOP API (aka OGS API) to interact with the engine
* Ability to use external window that was passed to engine (or it will be manually created on engine or renderdll (or some additional video module) level)
* All design is oriented on ability to change all own impl to SDL (or some other lib) later without any problem
* Single code for platform-independent window creation and interaction

```cpp
IWindow *IVideo::OpenWindow(const TWindowParams &aWinParams)

IWindow *pWindow = pVideo->OpenWindow(MainWindowParams);

pWindow->SetTitle("Die Freeman");
pWindow->SetPos(0, 0);

pVideo->CloseWindow(pWindow);