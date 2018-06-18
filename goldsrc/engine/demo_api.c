/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

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

void DemoAPI_WriteBuffer(int size, byte *buffer){
	// TODO
};

demo_api_t demoapi =
{
  DemoAPI_IsRecording,
  DemoAPI_IsPlayingback,
  DemoAPI_IsTimeDemo,
  DemoAPI_WriteBuffer
};