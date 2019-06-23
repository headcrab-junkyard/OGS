# Engine metamod module integration draft

**Approach #1:**
* Add special class for game dll (CGameHandler)
* Call all gamedll funcs through it

```cpp
void CGameHandler::Frame()
{
    pMetamod->Frame(); // call plugins preframe -> gamedll frame -> plugins postframe
};
```

Instead of direct gamedll function calling, we should call it via some intermediate level mechanism

This approach will allow to:
* Integrate the engine with metamod;
* Call new api game dll from the old ac96 arch;
* (Something else?)

> Engine -> game protocol -> gamedll/metamod plugins/new game dll/whatever you want, it mostly just an event...

**Approach #2:**
* Replace the current implementation of gamedll with local set of functions that will be used as mediator (MetaMod works this way, so it's possible to just use it's impl of gamedll). This approach would require the minimum changes in current code

> Engine -> Metamod impl of gEntityInterface -> Metamod plugins/Game DLL