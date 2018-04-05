/// @file
/// @brief demo api implementation

#include "quakedef.h"
#include "demo_api.h"

int DemoAPI_IsRecording()
{
	return cls.demorecording;
};

int DemoAPI_IsPlayingback()
{
	return cls.demoplayback;
};

int DemoAPI_IsTimeDemo()
{
	return cls.timedemo;
};

void DemoAPI_WriteBuffer(int size, byte *buffer)
{
	// TODO
};

demo_api_t demoapi =
{
	DemoAPI_IsRecording,
	DemoAPI_IsPlayingback,
	DemoAPI_IsTimeDemo,
	DemoAPI_WriteBuffer
};