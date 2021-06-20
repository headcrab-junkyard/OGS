/*
 * This file is part of OGS Engine
 * Copyright (C) 2021 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"

#include "voice/ivoicecodec.h"

void *gpVoiceCodecModule{nullptr};
IVoiceCodec *gpVoiceCodec{nullptr};

void Voice_UnloadCodec()
{
	if(gpVoiceCodecModule)
	{
		Sys_UnloadModule(gpVoiceCodecModule);
		gpVoiceCodecModule = nullptr;
	};
};

void Voice_LoadCodec(const char *name)
{
	Voice_UnloadCodec();
	
	gpVoiceCodecModule = Sys_LoadModule(name);
	
	if(gpVoiceCodecModule)
		Sys_Error(va("Failed to load the voice codec (%s)!", name));
	
	auto pVoiceCodecModuleFactory{Sys_GetFactory(gpVoiceCodecModule)};
	
	if(!pVoiceCodecModuleFactory)
		Sys_Error("Failed to get the voice codec module factory!");
	
	gpVoiceCodec = reinterpret_cast<IVoiceCodec*>(pVoiceCodecModuleFactory(OGS_VOICECODEC_INTERFACE_VERSION, nullptr));
	
	if(!gpVoiceCodec)
		Sys_Error("Failed to get the voice codec interface!");
};