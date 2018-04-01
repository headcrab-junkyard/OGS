/// @file

#include "DedicatedExports.h"

EXPOSE_SINGLE_INTERFACE(CDedicatedExports, IDedicatedExports, VENGINE_DEDICATEDEXPORTS_API_VERSION);

CDedicatedExports::CDedicatedExports() = default;
CDedicatedExports::~CDedicatedExports() = default;

void CDedicatedExports::Sys_Printf(char *text)
{
	// TODO
};