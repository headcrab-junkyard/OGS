# Engine Abstraction Levels

### EAL (Engine Abstraction Levels):
* Higher level hides it's lower-level implementation and provides a simple
and comfortable interface for it (Engine::Init/Shutdown)
* Lowest level has basis of common components (such as various utility code,
memory management, math, fs, system code and etc). It doesn't depends on any lower level of our code (in other words it still depends on platform code and such)

### Highest to Lowest:
(Lower levels doesn't depends on higher;
They depends on the same level or on lower)
* game, script
* network, input, physics, sound, graphics, resource formats
* system, memory, math, filesystem