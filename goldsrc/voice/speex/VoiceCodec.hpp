/*
 * This file is part of OGS Engine
 * Copyright (C) 2019, 2021 BlackPhrase
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

#pragma once

#include <speex/speex.h>

#include "voice/ivoicecodec.h"

class CVoiceCodecSpeex final : public IVoiceCodec
{
public:
	CVoiceCodecSpeex();
	~CVoiceCodecSpeex();
	
	bool Init(int anQuality) override;
	void Shutdown() override;
	
	void Encode();
	void Decode();
private:
	SpeexBits mBits{};
	
	void *mpEncState{nullptr}; ///< Holds the state of the encoder
	void *mpDecState{nullptr}; ///< Holds the state of the decoder
};