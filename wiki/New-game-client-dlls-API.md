# New gamedll/clientdll APIs

There will be the new APIs for both the game and client dlls  
New OGS API interfaces for engine<->dll interaction will be used  

### These interfaces are:

client->engine
* IClientDLL - main clientdll component that should init/shutdown and update the whole client dll stuff

game->engine
* IGameDLL/IGame - main component of the gamedll for same purposes