/// @file

#include "quakedef.h"

cvar_t voice_loopback = {"voice_loopback", "0"};

void Voice_Init()
{
	// TODO
};

void Voice_Shutdown()
{
	// TODO
};

void Voice_RegisterCvars()
{
	// TODO
	Cvar_RegisterVariable(&voice_loopback);
};