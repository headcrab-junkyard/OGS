/// @file
/// @brief vgui interface

#pragma once

//#ifdef __cplusplus
extern "C"
{
//#endif

void VGui_Startup();
void VGui_Shutdown();

/// Only safe to call from inside subclass of Panel::paintBackground
void VGui_ViewportPaintBackground(int extents[4]);

void VGui_ConsolePrint(const char *text);

void *VGui_GetPanel();

//#ifdef __cplusplus
};
//#endif