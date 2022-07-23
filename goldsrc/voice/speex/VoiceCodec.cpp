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

#include "VoiceCodec.hpp"

EXPOSE_SINGLE_INTERFACE(CVoiceCodecSpeex, IVoiceCodec, OGS_VOICECODEC_INTERFACE_VERSION);

CVoiceCodecSpeex::CVoiceCodecSpeex() = default;
CVoiceCodecSpeex::~CVoiceCodecSpeex() = default;

bool CVoiceCodecSpeex::Init(int anQuality)
{
	mpEncState{speex_encoder_init(&speex_nb_mode)};
	mpDecState{speex_decoder_init(&speex_nb_mode)};
	
	speex_bits_init(&mBits);
	
	speex_encoder_ctl(mpEncState, SPEEX_SET_QUALITY, &anQuality);
	
	return true;
};

void CVoiceCodecSpeex::Shutdown()
{
	speex_bits_destroy(&mBits);
	
	speex_decoder_destroy(pDecState);
	speex_encoder_destroy(pEncState);
};

void CVoiceCodecSpeex::Encode()
{
	speex_bits_reset(&mBits);
	
	speex_encode_int(mpEncState, input_frame, &mBits);
	
	nbBytes = speex_bits_write(&mBits, byte_ptr, MAX_NB_BYTES);
};

void CVoiceCodecSpeex::Decode()
{
	speex_bits_read_from(&mBits, input_bytes, nbBytes);
	
	speex_decode_int(mpDecState, &mBits, output_frame);
};