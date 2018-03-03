/// @file

#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// TEMP DUPLICATE
struct quakeparms_t
{
	char	*basedir;
	char	*cachedir;		// for development over ISDN lines
	int		argc;
	char	**argv;
	void	*membase;
	int		memsize;
};

struct IBaseInterface
{
};

using pfnHost_Init = void (*)(quakeparms_t *parms);
using pfnHost_Frame = void (*)(float frametime);
using pfnHost_Shutdown = void (*)();

using CreateInterfaceFn = IBaseInterface *(*)(const char *name, int *retval);

constexpr auto CREATEINTERFACE_PROCNAME{"CreateInterface"};

void *Sys_LoadModule(const char *name)
{
#ifdef _WIN32
	return (void*)LoadLibrary(name);
#else
	return dlopen(name, RTLD_NOW);
#endif
};

void *Sys_GetExport(void *apModule, const char *proc)
{
#ifdef _WIN32
	return (void*)GetProcAddress((HMODULE)apModule, proc);
#else
	return dlsym(apModule, proc);
#endif
};

CreateInterfaceFn Sys_GetFactory(void *apModule)
{
	return (CreateInterfaceFn)Sys_GetExport(apModule, CREATEINTERFACE_PROCNAME);
};

void Sys_UnloadModule(void *apModule)
{
#ifdef _WIN32
	FreeLibrary((HMODULE)apModule);
#else
	dlclose(apModule);
#endif
};

int main(int argc, char **argv)
{
	pfnHost_Init fnHost_Init{nullptr};
	pfnHost_Frame fnHost_Frame{nullptr};
	pfnHost_Shutdown fnHost_Shutdown{nullptr};
	
	auto pEngineLib{Sys_LoadModule("engine")};
	
	if(!pEngineLib)
		return EXIT_FAILURE;
	
	auto pEngineFactory{Sys_GetFactory(pEngineLib)};
	
	if(!pEngineFactory)
		return EXIT_FAILURE;
	
	fnHost_Init = (pfnHost_Init)Sys_GetExport(pEngineLib, "Host_Init");
	fnHost_Frame = (pfnHost_Frame)Sys_GetExport(pEngineLib, "Host_Frame");
	fnHost_Shutdown = (pfnHost_Shutdown)Sys_GetExport(pEngineLib, "Host_Shutdown");
	
	if(!fnHost_Init || !fnHost_Frame || !fnHost_Shutdown)
		return EXIT_FAILURE;
	
	//pEngineFactory();
	
	quakeparms_t parms{};
	
	parms.argc = argc;
	parms.argv = argv;
	
	fnHost_Init(&parms);
	
	while(true)
		fnHost_Frame(0.1f);
	
	fnHost_Shutdown();
	
	return EXIT_SUCCESS;
};