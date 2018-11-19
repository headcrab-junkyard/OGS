/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2018 BlackPhrase

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/// @file

#pragma once

#include <stdint.h>

#include "const.h"

typedef uint32_t CRC32_t;

void CRC32_Init(CRC32_t *crcvalue);
CRC32_t CRC32_Final(CRC32_t crcvalue);
void CRC32_ProcessBuffer(CRC32_t *crcvalue, void *data, int len);
void CRC32_ProcessByte(CRC32_t *crcvalue, unsigned char data);

//int CRC_File(CRC32_t *crcvalue, const char *pszFileName); // TODO

CRC32_t CRC32_Value(CRC32_t crcvalue);
CRC32_t CRC32_Block (byte *start, int count);

byte COM_BlockSequenceCRCByte(byte *base, int length, int sequence);

/* md5.h - header file for md5.c */
/* RSA Data Security, Inc., MD5 Message-Digest Algorithm */

/* NOTE: Numerous changes have been made; the following notice is
included to satisfy legal requirements.

Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991.

All rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/

typedef unsigned long UINT4;

// MD5 context
typedef struct
{
	UINT4 state[4]; ///< state (ABCD)
	UINT4 count[2]; ///< number of bits, modulo 2^64 (lsb first)
	unsigned char buffer[64]; ///< input buffer
} MD5Context_t;

void MD5Init(MD5Context_t *ctx);
void MD5Update(MD5Context_t *ctx, const unsigned char *input, unsigned int inputLen); // TODO: const void * -> const unsigned char *
void MD5Final(unsigned char digest[16], MD5Context_t *ctx);
void MD5Transform(UINT4 state[4], const unsigned char block[64]);