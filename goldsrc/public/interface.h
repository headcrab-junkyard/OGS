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

#pragma once

#include <vector>

#define EXPOSE_INTERFACE_FN(FunctionName, InterfaceName, VersionName)\
	static InterfaceRegistryEntry(VersionName, FunctionName);

#define EXPOSE_INTERFACE(ClassName, InterfaceName, VersionName)\
	static IBaseInterface *Create##ClassName##InterfaceName##Instance()\
	{\
		return (IBaseInterface*)new ClassName;\
	};\
	static InterfaceRegistryEntry gCreate##ClassName##RegEntry(VersionName, Create##ClassName##InterfaceName##Instance);

#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ClassName, InterfaceName, VersionName, Var)\
	static IBaseInterface *Create##ClassName##InterfaceName##Instance()\
	{\
		return (IBaseInterface*)&Var;\
	};\
	static InterfaceRegistryEntry gCreate##ClassName##InterfaceName##RegEntry(VersionName, Create##ClassName##InterfaceName##Instance);

#define EXPOSE_SINGLE_INTERFACE(ClassName, InterfaceName, VersionName)\
	static ClassName g_##ClassName##_;\
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ClassName, InterfaceName, VersionName, g_##ClassName##_)

/// Export name
constexpr auto CREATEINTERFACE_PROCNAME{"CreateInterface"};

/// Base interface; all interfaces should derive from this
struct IBaseInterface
{
	// Hi there!
};

/// Export function prototype
using CreateInterfaceFn = IBaseInterface *(*)(const char *name, int *retval);

/// Loads a module
///
/// @param name - name of the module
/// @return a valid pointer to the module
void *Sys_LoadModule(const char *name);

/// Gets an export function from a module
///
/// @param apModule - pointer to the module
/// @param proc - name of the export function
void *Sys_GetExport(void *apModule, const char *proc);

/// Gets an interface factory from a module
///
/// @param apModule - pointer to the module
CreateInterfaceFn Sys_GetFactory(void *apModule);

/// Gets an interface factory from this module (the module this function is called from)
CreateInterfaceFn Sys_GetFactoryThis();

/// Unloads a module
///
/// @param apModule - pointer to the module to unload
void Sys_UnloadModule(void *apModule);

using pfnInstantiate = IBaseInterface *(*)();

struct InterfaceRegistryEntry;
using tInterfaceRegEntryVec = std::vector<InterfaceRegistryEntry*>;

struct InterfaceRegistryEntry
{
	InterfaceRegistryEntry(const char *asName, pfnInstantiate afnInstantiate)
		: msName(asName), mfnInstantiate(afnInstantiate)
	{
		svEntries.push_back(this);
	};
	
	const char *msName{""};
	pfnInstantiate mfnInstantiate{nullptr};
	
	//InterfaceRegistryEntry *mpNext{nullptr};
	
	//static InterfaceRegistryEntry *spEntryList;
	static tInterfaceRegEntryVec svEntries;
};