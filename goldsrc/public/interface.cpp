/*
Copyright 2018 Headcrab Garage

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/// @file

#include <cstring>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "interface.h"

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

tInterfaceRegEntryVec InterfaceRegistryEntry::svEntries;

C_EXPORT IBaseInterface *CreateInterface(const char *name, int *retval)
{
	if(retval)
		*retval = 0;
	
	for(auto It : InterfaceRegistryEntry::svEntries)
		if(!strcmp(It->msName, name))
		{
			if(It->mfnInstantiate)
			{
				if(retval)
					*retval = 1;
				
				return It->mfnInstantiate();
			};
		};
	
	return nullptr;
};

void *Sys_LoadModule(const char *name)
{
	if(!name || !*name)
	{
		//printf("");
		//exit(EXIT_FAILURE);
		return nullptr;
	};
	
	void *pLib{nullptr};
	
#ifdef _WIN32
	pLib = (void*)LoadLibrary(name);
#else // if __linux__ or __APPLE__
	pLib = dlopen(name, RTLD_NOW); // TODO: RTLD_LAZY?
#endif

	if(!pLib)
	{
#ifdef _WIN32
		//printf("[%s] Unable to load a dynamic library \"%s\": %s\n", __FILE__, name, GetLastError());
		//system("pause");
#else
		//printf("[%s] Unable to load a dynamic library \"%s\": %s\n", __FILE__, name, dlerror());
		//exit(EXIT_FAILURE);
#endif
	};
	
	return pLib;
};

void *Sys_GetExport(void *apModule, const char *proc)
{
#ifdef _WIN32
	return (void*)GetProcAddress((HMODULE)apModule, proc);
#else // if __linux__ or __APPLE__
	return dlsym(apModule, proc);
#endif
};

CreateInterfaceFn Sys_GetFactory(void *apModule)
{
	return (CreateInterfaceFn)Sys_GetExport(apModule, CREATEINTERFACE_PROCNAME);
};

CreateInterfaceFn Sys_GetFactoryThis()
{
	return CreateInterface;
};

void Sys_UnloadModule(void *apModule)
{
#ifdef _WIN32
	FreeLibrary((HMODULE)apModule);
#else // if __linux__ or __APPLE__
	dlclose(apModule);
#endif
};