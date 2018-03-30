/// @file
/// @brief demo api implementation

#include "quakedef.h"
#include "demo_api.h"

int DemoAPI_IsRecording()
{
	return cl.recording;
};

int DemoAPI_IsPlayingback()
{
	return cl.playback;
};

int DemoAPI_IsTimeDemo()
{
	return cl.timedemo;
};

void DemoAPI_WriteBuffer(int size, byte *buffer)
{
};

demo_api_t demoapi =
{
	DemoAPI_IsRecording,
	DemoAPI_IsPlayingback,
	DemoAPI_IsTimeDemo,
	DemoAPI_WriteBuffer
};