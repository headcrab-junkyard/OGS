There are two VGUI wrapper layers:

* One is responsible for VGUI1
* The other handles everything related to VGUI2

* VGuiWrap_* functions are responsible for vgui library initialization and root panel creation and its events handling
* VGuiWrap2_* functions are responsible for interaction with stuff that's using VGUI2 (like GameUI module)
* VGui_* functions are just redirects to either VGuiWrap_*/VGuiWrap2_* functions or both of them at once
* VGuiWrap2_* functions wrap (almost?) every method of IGameUI/IGameConsole interfaces of the GameUI module, some methods of IBaseUI interface and some methods of IClientUI interface 
optionally exported from the client dll module