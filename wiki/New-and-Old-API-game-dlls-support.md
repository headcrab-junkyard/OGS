# How both OGS/Legacy API game dlls will be supported (Draft)

* Event system implementation
* EventManager polls (gets without blocking) or gets events and broadcasts 'em to other subsystems

```cpp
if(EntityEvent.meType == EntEvent_Spawn)
{
    // An entity has spawned
    // Do something...
};
```

* Events can be handled by the game dlls (since the new game dlls interfaces are generic and oriented mostly on init/shutdown and update them + ability to get interfaces from the engine module or some other submodules (i.e. console or fs) all it's need to be done is to get a handler interface in init method and register a new event listener through it)

```cpp
class CGameEntityEventListener : public IEntityEventListener
{
    void OnEvent(TEntityEvent &aEvent)
    {
        if(aEvent.type == eEntEvent_Spawn)
        {
            IEntity *pEntity = aEvent.entity;
            mpServer->BroadcastMessage("Entity \"%s\" (ID: %d) has spawned!", pEntity->GetName(), pEntity->GetID());
        };
    };
};

CGameEntityEventListener gGameEntityEventListener;

mpEntityManager->AddListener(&gGameEntityEventListener);
```

* Old gamedll/clientdll will be supported using the same approach (by wrapping them into event listeners that redirects calls to dll impl side

```cpp
class CLegacyEntityListener : public IEntityEventListener
{
public:
    CLegacyEntityListener(CLegacyGame *apGame) : mpGame(apGame){}

    void OnEvent(TEntityEvent &aEvent)
    {
        switch(aEvent.type)
        {
        case eEntEvent_Touch:
            mpGame->OnEntityTouch(aEvent.entity->ToEdict(), aEvent.touchdata.touchent->ToEdict());
            break;
        };
    };
private:
    CLegacyGame *mpGame{nullptr}; // wrapper around the old game dll api (for both DLL_FUNCTIONS and NEW_DLL_FUNCTIONS)
};
```

* The same approach can be applied even for plugins. All you need is to implement a listener interface, instantiate it and register using the appropriate handler
* **[USELESS]**: ILegacyGame interface (that represents the legacy game dll exports) and it's version specific implementation. Using this approach it'll be possible to support even the HLSDK1 gamedlls (Just need to implement the interface that will be using the old HLSDK1 game interface (It's just a reduced version of the current))
* Legacy game loader component which will detect the game interface version and could provide the appropriate export set to CLegacyGame (If it detect the HLSDK1 interface version the reduced function set to CLegacyGame will be passed)