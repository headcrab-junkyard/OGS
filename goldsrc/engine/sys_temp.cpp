/// @file

#include "tier1/interface.h"

extern "C"
{
void *Sys_GetExport_Wrapper(void *apModule, const char *asName)
{
	return Sys_GetExport(apModule, asName);
};
};