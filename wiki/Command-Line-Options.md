(Taken from [VDC](https://developer.valvesoftware.com/wiki/Command_Line_Options))

# Goldsource Games

These command-line parameters will work with any [Goldsource](https://developer.valvesoftware.com/wiki/Goldsource) engine games (Half-Life, Counter-Strike, etc.).

**Note:** Command parameters are described inside the `<` and `>` characters.

**Syntax:** `hl.exe <parameter1> <parameter2> <cvar1> <cvar2>`

## Usage

These options can be used with:

   * Hammer editor [Run Map](https://developer.valvesoftware.com/wiki/Hammer_Run_Map_Normal) dialog.
   * Windows shortcuts pointing to hl.exe directly.
   * Windows shortcuts pointing to steam.exe with the proper -applaunch <AppID> specified (see [Steam](https://developer.valvesoftware.com/wiki/Command_Line_Options#Steam_.28Windows.29) section).
   * Command-line batch script files launching these applications.
   * Game launch options from Steam's Library.

## Examples

   * steam.exe -applaunch 70 -dev -console -sw +sv_lan 1 +map MyMap
   * hl.exe -game cstrike -dev -console -fullscreen +sv_lan 1 +map de_dust

## Command-line parameters

   * -game <[game](https://developer.valvesoftware.com/wiki/Game_Name_Abbreviations)> - Specifies which game/mod to run. Default is "valve".
   * -dll <name.dll> - Specifies which game DLL to use. Ex: -dll dlls/zbotcz.dll.
   * -condebug - Stores console output to "Half-Life\qconsole.log".
    Note:Can be toggled with condebug console command.
   * -dev - Enables developer mode.
   * -glext - When paired with "-dev" setting, outputs all OpenGL extensions supported by your graphics card.
   * Note:Can do the same with gl_dump console command.
   * -gl - Sets the rendered to OpenGL.
   * -soft - Sets the rendered to Software.
   * -full - Forces the engine to start in fullscreen mode.
   * -window or -sw or -startwindowed or -windowed - Forces the engine to start in windowed mode.
   * -w <width> or -width <width> - Forces the engine to start with resolution set to <width>. Ex: -w 1024.
   * -h <height> or -height <height>- Forces the engine to start with resolution set to <height>. Ex: -h 768.
   * -stretchaspect - The game by default adds black bars based on your game screen resolution. Use -stretchaspect on your command line if you want the old behavior back (just always fill the screen). Setting "-nofbo" also implies "-stretchaspect".
   * -freq <variable> - Force a specific refresh rate when running in fullscreen AND your monitor supports the frequency in question.
    Note:If you don't have a CRT monitor chances are you can't use this option.
   * -mousethread - If set we poll the mouse position in another thread running at 100hz by default, to get better movement accuracy. Set the "m_mousethread_sleep" cvar to the number of seconds to sleep between mouse polls to change this rate.
   * -joy_advanced - Use advanced joystick options (allows for multiple axes).
   * -joy_advr, -joy_advu, -joy_advv, -joy_advx, -joy_advy, -joy_advz - To do
   * -nosound - Disables sound support.
    Note:Can do the same with nosound <0 or 1> console command.
   * -nocdaudio - Disables mp3 support. Also disallows cd console command to load mp3 tracks.
   * -wavonly - Disables DirectSound and makes the engine attempt to play all sounds in PCM format.
   * -nointro - Disables intro movies.
   * -noip - Disables network support.
   * -noipx - Disables IPX support.
   * -nojoy - Disables joystick support.
   * -nomouse - Disables in-game mouse support.
   * -noforcemparms - Forces the engine to use operating system mouse parameters.
   * -noforcemaccel - Forces the engine to use operating system mouse acceleration.
   * -noforcemspd - Forces the engine to use operating system mouse speed.
   * -nofbo - Disables using the new method for rendering.
   * -nomsaa - Disables MSAA (Anti-Aliasing), setting "-nofbo" also implies "-nomsaa".
   * -noborder - When used with the game set to windowed mode, will make the game act as if in fullscreen mode (no window border).
    Bug:
    Doesn't hide window border, only control buttons.
   * -nodirectblit - Disables direct blitting (Blit is shorthand for "block image transfer" or bit blit).
   * -nobots - Disables bots.
   * -port <port> - Specifies which port to use for client connections for a listen server. Default is 27015.
    Note:Can do the same with port console command.
   * -demoedit - Enables hidden Demo editor.
    Note:Use viewdemo console command to get access.
   * -num_edicts <variable> - Sets the entity limit for map/game entities (not temporary entities, which is still locked to 500). 900 is default. 4096 is a good limit, beyond that seems to cause hunk_alloc errors[confirm]. Both clients and servers need matching values for best compatibility[confirm].
   * -heapsize <kilobytes> - Specifies the amount of heap(or free store - cache, an area of memory used for dynamic memory allocation) the engine will use. Minimum value is 14336(14 MB). Maximum value is 131072(128 MB). By default this is set to 40960 (40 MB) and automatically adjusted to suit your system.
    Note:Low value causes "Unable to allocate X.X MB" exit error.
   * -zone <bytes> - Specifies the amount of Bytes of memory for use with the console system.
    Note:Low or very high value causes [Z_Malloc](https://developer.valvesoftware.com/wiki/Z_malloc) exit error.
   * -exec <file> - Executes specific config file immediately after the engine is loaded.
    +<console variable> <arg> - Sets the Cvar (or [console variable](https://developer.valvesoftware.com/wiki/Console_variable)) to the specified setting.

### Useful console variables

   * +_setgamedir <[game](https://developer.valvesoftware.com/wiki/Game_Name_Abbreviations)> - Specifies which game/mod to run.
   * +_setrenderer <variable> - Sets the rendering mode. Available parameters are opengl and software.
    Note:Parameter D3D is deprecated.
   * +_setvideomode - To do
   * +_set_vid_level <0 or 1> - Toogles "Low video quality".
   * +_setaddons_folder <0 or 1> - Toogles "Allow custom addon content".
   * +_sethdmodels <0 or 1> - Toogles "Enable HD models if available".
   * +clientport <port> - Specifies which port to use to communicate with a server. Default is 27005.
   * +gg <demoname> or +cl_gg 1 +playdemo <demoname> - Runs the demo in Game Gauge mode. This is a benchmark utility that runs through a demo as fast as possible and stores the framerate in the file fps.txt in the GameMod directory.
   * +log on or +log off - Toogles logging before the engine loads config files.

### Deprecated command-line parameters

   * -console - Enables console. Can be opened either from menu (button in top-left corner in old Half-Life builds or button in main menu in new builds), or by pressing tilde (~) in-game.
    Note:Always enabled by default.
   * -toconsole - Opens the console right when the game is launched.
    Note:No longer works.
   * -gldrv <driver name> - Selects the GL Driver. The two choices are "Default" and "3dfxgl.dll".
    Note:No longer works.
   * -d3d - Sets the rendered to Direct3D.
    Note:Removed.[1]
   * -vid_mode <variable> or -vt <variable> - Sets the rendering mode. Available parameters are gl and software.
    Note:No logner works. Use -gl, -soft or -d3d(see above) instead.
   * -16bpp - Forces 16-bit color mode (bit depth).
    Note:Not allowed.
   * -32bpp - Forces 32-bit color mode (bit depth).
    Note:Enabled by default.
   * -nowinmouse - Don't capture mouse in-game.
    Note:Renamed to -nomouse.
   * -gamegauge <demoname> - Runs the demo in Game Gauge mode. This is a benchmark utility that runs through a demo as fast as possible and stores the framerate in the file fps.txt in the GameMod directory.
    Note:Use console commands instead.
   * -numericping - Displays ping in server browsers in milliseconds instead of dots.
    Note:Enabled by default.
   * -particles <variable> - The number of particles for which to set aside buffer space.
    Note:No longer works.
   * -nosierra - [confirm] To do
   * -noaff - Disables setting of thread affinity.
    Note:No longer works.
