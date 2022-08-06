/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2005 Id Software, Inc.
 * Copyright (C) 2018, 2020-2022 BlackPhrase
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
/// @brief misc functions used in client and server

#include "quakedef.h"

#define NUM_SAFE_ARGVS 7

static char *largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1];
static char *argvdummy = " ";

static char *safeargvs[NUM_SAFE_ARGVS] =
{ "-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse", "-dibonly" };

qboolean msg_suppress_1 = false;

void COM_InitFilesystem();

char com_token[1024];
int com_argc;
char **com_argv;

#define CMDLINE_LENGTH 256
char com_cmdline[CMDLINE_LENGTH];

/*


All of engine's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the hl.exe and all game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.  This can be overridden with the "-basedir" command line parm to allow code debugging in a different directory.  The base directory is
only used during filesystem initialization.

The "game directory" is the first tree on the search path and directory that all generated files (savegames, screenshots, demos, config files) will be saved to.  This can be overridden with the "-game" command line parameter.  The game directory can never be changed while quake is executing.  This is a precacution against having a malicious server instruct clients to write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth, especially over ISDN / T1 lines.  If there is a cache directory
specified, when a file is found by the normal search path, it will be mirrored
into the cache directory, then opened there.



FIXME:
The file "parms.txt" will be read out of the game directory and appended to the current command line arguments to allow different games to initialize startup parms differently.  This could be used to add a "-sspeed 22050" for the high quality sound edition.  Because they are added at the end, they will not override an explicit setting on the original command line.
	
*/

//============================================================================

// ClearLink is used for new headnodes
void ClearLink(link_t *l)
{
	l->prev = l->next = l;
}

void RemoveLink(link_t *l)
{
	l->next->prev = l->prev;
	l->prev->next = l->next;
}

void InsertLinkBefore(link_t *l, link_t *before)
{
	l->next = before;
	l->prev = before->prev;
	l->prev->next = l;
	l->next->prev = l;
}
void InsertLinkAfter(link_t *l, link_t *after)
{
	l->next = after->next;
	l->prev = after;
	l->prev->next = l;
	l->next->prev = l;
}

/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

void Q_memset(void *dest, int fill, int count)
{
	int i;

	if((((long)dest | count) & 3) == 0)
	{
		count >>= 2;
		fill = fill | (fill << 8) | (fill << 16) | (fill << 24);
		for(i = 0; i < count; i++)
			((int *)dest)[i] = fill;
	}
	else
		for(i = 0; i < count; i++)
			((byte *)dest)[i] = fill;
}

void Q_memcpy(void *dest, const void *src, int count)
{
	int i;

	if((((long)dest | (long)src | count) & 3) == 0)
	{
		count >>= 2;
		for(i = 0; i < count; i++)
			((int *)dest)[i] = ((int *)src)[i];
	}
	else
		for(i = 0; i < count; i++)
			((byte *)dest)[i] = ((byte *)src)[i];
}

int Q_memcmp(const void *m1, const void *m2, int count)
{
	while(count)
	{
		count--;
		if(((byte *)m1)[count] != ((byte *)m2)[count])
			return -1;
	}
	return 0;
}

void Q_strcpy(char *dest, const char *src)
{
	while(*src)
	{
		*dest++ = *src++;
	}
	*dest++ = 0;
}

void Q_strncpy(char *dest, const char *src, int count)
{
	while(*src && count--)
	{
		*dest++ = *src++;
	}
	if(count)
		*dest++ = 0;
}

int Q_strlen(const char *str)
{
	int count;

	count = 0;
	while(str[count])
		count++;

	return count;
}

const char *Q_strrchr(const char *s, char c)
{
	int len = Q_strlen(s);
	s += len;

	while(len--)
		if(*--s == c)
			return s;

	return 0;
}

void Q_strcat(char *dest, const char *src)
{
	dest += Q_strlen(dest);
	Q_strcpy(dest, src);
}

int Q_strcmp(const char *s1, const char *s2)
{
	while(1)
	{
		if(*s1 != *s2)
			return -1; // strings not equal
		if(!*s1)
			return 0; // strings are equal
		s1++;
		s2++;
	}

	return -1;
}

int Q_strncmp(const char *s1, const char *s2, int count)
{
	while(1)
	{
		if(!count--)
			return 0;
		if(*s1 != *s2)
			return -1; // strings not equal
		if(!*s1)
			return 0; // strings are equal
		s1++;
		s2++;
	}

	return -1;
}

int Q_strncasecmp(const char *s1, const char *s2, int n)
{
	int c1, c2;

	while(1)
	{
		c1 = *s1++;
		c2 = *s2++;

		if(!n--)
			return 0; // strings are equal until end point

		if(c1 != c2)
		{
			if(c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if(c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
			if(c1 != c2)
				return -1; // strings not equal
		}
		if(!c1)
			return 0; // strings are equal
		
		//s1++;
		//s2++;
	}

	return -1;
}

int Q_strcasecmp(const char *s1, const char *s2)
{
	return Q_strncasecmp(s1, s2, 99999);
}

int Q_atoi(const char *str)
{
	int val;
	int sign;
	int c;

	if(*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

	//
	// check for hex
	//
	if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
		while(1)
		{
			c = *str++;
			if(c >= '0' && c <= '9')
				val = (val << 4) + c - '0';
			else if(c >= 'a' && c <= 'f')
				val = (val << 4) + c - 'a' + 10;
			else if(c >= 'A' && c <= 'F')
				val = (val << 4) + c - 'A' + 10;
			else
				return val * sign;
		}
	}

	//
	// check for character
	//
	if(str[0] == '\'')
	{
		return sign * str[1];
	}

	//
	// assume decimal
	//
	while(1)
	{
		c = *str++;
		if(c < '0' || c > '9')
			return val * sign;
		val = val * 10 + c - '0';
	}

	return 0;
}

float Q_atof(const char *str)
{
	double val;
	int sign;
	int c;
	int decimal, total;

	if(*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

	//
	// check for hex
	//
	if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
		while(1)
		{
			c = *str++;
			if(c >= '0' && c <= '9')
				val = (val * 16) + c - '0';
			else if(c >= 'a' && c <= 'f')
				val = (val * 16) + c - 'a' + 10;
			else if(c >= 'A' && c <= 'F')
				val = (val * 16) + c - 'A' + 10;
			else
				return val * sign;
		}
	}

	//
	// check for character
	//
	if(str[0] == '\'')
	{
		return sign * str[1];
	}

	//
	// assume decimal
	//
	decimal = -1;
	total = 0;
	while(1)
	{
		c = *str++;
		if(c == '.')
		{
			decimal = total;
			continue;
		}
		if(c < '0' || c > '9')
			break;
		val = val * 10 + c - '0';
		total++;
	}

	if(decimal == -1)
		return val * sign;
	while(total > decimal)
	{
		val /= 10;
		total--;
	}

	return val * sign;
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean bigendien;

short (*BigShort)(short l);
short (*LittleShort)(short l);
int (*BigLong)(int l);
int (*LittleLong)(int l);
float (*BigFloat)(float l);
float (*LittleFloat)(float l);

short ShortSwap(short l)
{
	byte b1, b2;

	b1 = l & 255;
	b2 = (l >> 8) & 255;

	return (b1 << 8) + b2;
}

short ShortNoSwap(short l)
{
	return l;
}

int LongSwap(int l)
{
	byte b1, b2, b3, b4;

	b1 = l & 255;
	b2 = (l >> 8) & 255;
	b3 = (l >> 16) & 255;
	b4 = (l >> 24) & 255;

	return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

int LongNoSwap(int l)
{
	return l;
}

float FloatSwap(float f)
{
	union
	{
		float f;
		byte b[4];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap(float f)
{
	return f;
}

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

static huffman_t msgHuff;

usercmd_t nullcmd; // guaranteed to be zero

//
// writing functions
//

void MSG_WriteChar(sizebuf_t *sb, int c)
{
	byte *buf;

#ifdef PARANOID
	if(c < -128 || c > 127)
		Sys_Error("MSG_WriteChar: range error");
#endif

	buf = SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteByte(sizebuf_t *sb, int c)
{
	byte *buf;

#ifdef PARANOID
	if(c < 0 || c > 255)
		Sys_Error("MSG_WriteByte: range error");
#endif

	buf = SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteShort(sizebuf_t *sb, int c)
{
	byte *buf;

#ifdef PARANOID
	if(c < ((short)0x8000) || c > (short)0x7fff)
		Sys_Error("MSG_WriteShort: range error");
#endif

	buf = SZ_GetSpace(sb, 2);
	buf[0] = c & 0xff;
	buf[1] = c >> 8;
}

void MSG_WriteLong(sizebuf_t *sb, int c)
{
	byte *buf;

	buf = SZ_GetSpace(sb, 4);
	buf[0] = c & 0xff;
	buf[1] = (c >> 8) & 0xff;
	buf[2] = (c >> 16) & 0xff;
	buf[3] = c >> 24;
}

void MSG_WriteFloat(sizebuf_t *sb, float f)
{
	union
	{
		float f;
		int l;
	} dat;

	dat.f = f;
	dat.l = LittleLong(dat.l);

	SZ_Write(sb, &dat.l, 4);
}

void MSG_WriteString(sizebuf_t *sb, const char *s)
{
	if(!s)
		SZ_Write(sb, "", 1);
	else
		SZ_Write(sb, s, Q_strlen(s) + 1);
}

void MSG_WriteCoord(sizebuf_t *sb, float f)
{
	MSG_WriteShort(sb, (int)(f * 8));
}

void MSG_WriteAngle(sizebuf_t *sb, float f)
{
	MSG_WriteByte(sb, ((int)f * 256 / 360) & 255);
}

void MSG_WriteBuf(sizebuf_t *buf, const void *data, int length)
{
	for(int i = 0; i < length; i++)
		MSG_WriteByte(buf, ((byte *)data)[i]);
}

//============================================================

//
// reading functions
//

int msg_readcount;
qboolean msg_badread;

void MSG_BeginReading()
{
	msg_readcount = 0;
	msg_badread = false;
}

int MSG_GetReadCount()
{
	return msg_readcount;
}

int MSG_ReadBits(int bits)
{
	int			value;
	int			get;
	qboolean	sgn;
	int			i, nbits;
//	FILE*	fp;

	value = 0;

	if ( bits < 0 )
	{
		bits = -bits;
		sgn = true;
	}
	else
		sgn = false;

	if (net_message.oob)
	{
		if (bits==8)
		{
			value = net_message.data[msg_readcount];
			msg_readcount += 1;
			net_message.bit += 8;
		}
		else if(bits == 16)
		{
			unsigned short *sp = (unsigned short *)&net_message.data[msg_readcount];
			value = LittleShort(*sp);
			msg_readcount += 2;
			net_message.bit += 16;
		}
		else if (bits==32)
		{
			unsigned int *ip = (unsigned int *)&net_message.data[msg_readcount];
			value = LittleLong(*ip);
			msg_readcount += 4;
			net_message.bit += 32;
		}
		else
			Host_Error(/*ERR_DROP,*/ "can't read %d bits\n", bits); // TODO: Sys_Error?
	}
	else
	{
		nbits = 0;
		if (bits & 7)
		{
			nbits = bits & 7;
			for(i=0; i < nbits; i++)
				value |= (Huff_getBit(net_message.data, &net_message.bit)<<i);

			bits -= nbits;
		}
		
		if (bits)
		{
//			fp = fopen("c:\\netchan.bin", "a");
			for(i=0; i < bits; i += 8)
			{
				Huff_offsetReceive (msgHuff.decompressor.tree, &get, net_message.data, &net_message.bit);
//				fwrite(&get, 1, 1, fp);
				value |= (get << (i + nbits));
			}
//			fclose(fp);
		}
		msg_readcount = (net_message.bit>>3)+1;
	}
	if ( sgn )
		if ( value & ( 1 << ( bits - 1 ) ) )
			value |= -1 ^ ( ( 1 << bits ) - 1 );

	return value;
}

int MSG_ReadOneBit()
{
	return MSG_ReadBits(1);
};

// returns -1 and sets msg_badread if no more characters are available
int MSG_ReadChar()
{
	int c;

	if(msg_readcount + 1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (signed char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadByte()
{
	int c;

	if(msg_readcount + 1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (unsigned char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadShort()
{
	int c;

	if(msg_readcount + 2 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (short)(net_message.data[msg_readcount] + (net_message.data[msg_readcount + 1] << 8));

	msg_readcount += 2;

	return c;
}

int MSG_ReadWord()
{
	int c;

	if(msg_readcount + 3 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (int)(net_message.data[msg_readcount] + (net_message.data[msg_readcount + 1] << 8) + (net_message.data[msg_readcount + 2] << 16));

	msg_readcount += 3;

	return c;
}

int MSG_ReadLong()
{
	int c;

	if(msg_readcount + 4 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = net_message.data[msg_readcount] + (net_message.data[msg_readcount + 1] << 8) + (net_message.data[msg_readcount + 2] << 16) + (net_message.data[msg_readcount + 3] << 24);

	msg_readcount += 4;

	return c;
}

float MSG_ReadFloat()
{
	union
	{
		byte b[4];
		float f;
		int l;
	} dat;

	dat.b[0] = net_message.data[msg_readcount];
	dat.b[1] = net_message.data[msg_readcount + 1];
	dat.b[2] = net_message.data[msg_readcount + 2];
	dat.b[3] = net_message.data[msg_readcount + 3];
	msg_readcount += 4;

	dat.l = LittleLong(dat.l);

	return dat.f;
}

char *MSG_ReadString()
{
	static char string[2048];
	int l, c;

	l = 0;
	do
	{
		c = MSG_ReadChar();
		if(c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while(l < sizeof(string) - 1);

	string[l] = 0;

	return string;
}

char *MSG_ReadStringLine()
{
	static char	string[2048];
	int		l,c;
	
	l = 0;
	do
	{
		c = MSG_ReadChar ();
		if (c == -1 || c == 0 || c == '\n')
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

float MSG_ReadCoord()
{
	return MSG_ReadShort() * (1.0 / 8);
}

float MSG_ReadAngle()
{
	return MSG_ReadChar() * (360.0 / 256);
}

void MSG_ReadDeltaUsercmd(usercmd_t *from, usercmd_t *move)
{
	// TODO
/*
	int bits;

	memcpy (move, from, sizeof(*move));

	bits = MSG_ReadByte ();
		
// read current angles
	if (bits & CM_ANGLE1)
		move->angles[0] = MSG_ReadAngle16 ();
	if (bits & CM_ANGLE2)
		move->angles[1] = MSG_ReadAngle16 ();
	if (bits & CM_ANGLE3)
		move->angles[2] = MSG_ReadAngle16 ();
		
// read movement
	if (bits & CM_FORWARD)
		move->forwardmove = MSG_ReadShort ();
	if (bits & CM_SIDE)
		move->sidemove = MSG_ReadShort ();
	if (bits & CM_UP)
		move->upmove = MSG_ReadShort ();
	
// read buttons
	if (bits & CM_BUTTONS)
		move->buttons = MSG_ReadByte ();

	if (bits & CM_IMPULSE)
		move->impulse = MSG_ReadByte ();

// read time to run command
	move->msec = MSG_ReadByte ();
*/
}

//===========================================================================

void SZ_Alloc(sizebuf_t *buf, int startsize)
{
	if(startsize < 256)
		startsize = 256;
	buf->data = Hunk_AllocName(startsize, "sizebuf");
	buf->maxsize = startsize;
	buf->cursize = 0;
}

void SZ_Free(sizebuf_t *buf)
{
	//      Z_Free (buf->data);
	//      buf->data = NULL;
	//      buf->maxsize = 0;
	buf->cursize = 0;
}

void SZ_Clear(sizebuf_t *buf)
{
	buf->cursize = 0;
}

void *SZ_GetSpace(sizebuf_t *buf, int length)
{
	void *data;

	if(buf->cursize + length > buf->maxsize)
	{
		if(!buf->allowoverflow)
			Sys_Error("SZ_GetSpace: overflow without allowoverflow set");

		if(length > buf->maxsize)
			Sys_Error("SZ_GetSpace: %i is > full buffer size", length);

		buf->overflowed = true;
		Con_Printf("SZ_GetSpace: overflow");
		SZ_Clear(buf);
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void SZ_Write(sizebuf_t *buf, const void *data, int length)
{
	Q_memcpy(SZ_GetSpace(buf, length), data, length);
}

void SZ_Print(sizebuf_t *buf, const char *data)
{
	int len;

	len = Q_strlen(data) + 1;

	// byte * cast to keep VC++ happy
	if(buf->data[buf->cursize - 1])
		Q_memcpy((byte *)SZ_GetSpace(buf, len), data, len); // no trailing 0
	else
		Q_memcpy((byte *)SZ_GetSpace(buf, len - 1) - 1, data, len); // write over trailing 0
}

//============================================================================

/*
============
COM_SkipPath
============
*/
const char *COM_SkipPath(const char *pathname)
{
	const char *last;

	last = pathname;
	while(*pathname)
	{
		if(*pathname == '/')
			last = pathname + 1;
		pathname++;
	}
	return last;
}

int COM_ExpandFilename(const char *asFileName, char *asNameOutBuffer, int anOutBufferSize)
{
	return 0; // TODO
};

/*
============
COM_StripExtension
============
*/
void COM_StripExtension(const char *in, char *out)
{
	while(*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension(const char *in)
{
	static char exten[8];
	int i;

	while(*in && *in != '.')
		in++;
	if(!*in)
		return "";
	in++;
	for(i = 0; i < 7 && *in; i++, in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
void COM_FileBase(const char *in, char *out)
{
	const char *s, *s2;

	s = in + strlen(in) - 1;

	while(s != in && *s != '.')
		s--;

	for(s2 = s; *s2 && *s2 != '/' && *s2 != '\\'; s2--)
		;

	if(s - s2 < 2)
		strcpy(out, "?model?");
	else
	{
		s--;
		strncpy(out, s2 + 1, s - s2);
		out[s - s2] = 0;
	}
}

/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension(char *path, const char *extension)
{
	char *src;
	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	src = path + strlen(path) - 1;

	while(*src != '/' && src != path)
	{
		if(*src == '.')
			return; // it has an extension
		src--;
	}

	strcat(path, extension);
}

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
const char *COM_Parse(const char *data)
{
	int c;
	int len;

	len = 0;
	com_token[0] = 0;

	if(!data)
		return NULL;

// skip whitespace
skipwhite:
	while((c = *data) <= ' ')
	{
		if(c == 0)
			return NULL; // end of file;
		data++;
	}

	// skip // comments
	if(c == '/' && data[1] == '/')
	{
		while(*data && *data != '\n')
			data++;
		goto skipwhite;
	}

	// handle quoted strings specially
	if(c == '\"')
	{
		data++;
		while(1)
		{
			c = *data++;
			if(c == '\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

	// parse single characters
	if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ':')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}

	// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ':')
			break;
	} while(c > 32);

	com_token[len] = 0;
	return data;
}

/*
================
COM_CheckParm

Returns the position (1 to argc-1) in the program's argument list
where the given parameter apears, or 0 if not present
================
*/
int COM_CheckParm(const char *parm)
{
	int i;

	for(i = 1; i < com_argc; i++)
	{
		if(!com_argv[i])
			continue; // NEXTSTEP sometimes clears appkit vars.
		if(!Q_strcmp(parm, com_argv[i]))
			return i;
	}

	return 0;
}

/*
================
COM_InitArgv
================
*/
void COM_InitArgv(int argc, char **argv)
{
	qboolean safe;
	int i, j, n;

	// reconstitute the command line for the cmdline externally visible cvar
	n = 0;

	for(j = 0; (j < MAX_NUM_ARGVS) && (j < argc); j++)
	{
		i = 0;

		while((n < (CMDLINE_LENGTH - 1)) && argv[j][i])
		{
			com_cmdline[n++] = argv[j][i++];
		}

		if(n < (CMDLINE_LENGTH - 1))
			com_cmdline[n++] = ' ';
		else
			break;
	}

	com_cmdline[n] = 0;

	safe = false;

	for(com_argc = 0; (com_argc < MAX_NUM_ARGVS) && (com_argc < argc);
	    com_argc++)
	{
		largv[com_argc] = argv[com_argc];
		if(!Q_strcmp("-safe", argv[com_argc]))
			safe = true;
	}

	if(safe)
	{
		// force all the safe-mode switches. Note that we reserved extra space in
		// case we need to add these, so we don't need an overflow check
		for(i = 0; i < NUM_SAFE_ARGVS; i++)
		{
			largv[com_argc] = safeargvs[i];
			com_argc++;
		}
	}

	largv[com_argc] = argvdummy;
	com_argv = largv;
}

/*
================
COM_Init
================
*/
void COM_Init(const char *basedir)
{
	byte swaptest[2] = { 1, 0 };

	// set the byte swapping variables in a portable manner
	if(*(short *)swaptest == 1)
	{
		bigendien = false;
		BigShort = ShortSwap;
		LittleShort = ShortNoSwap;
		BigLong = LongSwap;
		LittleLong = LongNoSwap;
		BigFloat = FloatSwap;
		LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien = true;
		BigShort = ShortNoSwap;
		LittleShort = ShortSwap;
		BigLong = LongNoSwap;
		LittleLong = LongSwap;
		BigFloat = FloatNoSwap;
		LittleFloat = FloatSwap;
	}

	COM_InitFilesystem();
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char *va(const char *format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

/// just for debugging
int memsearch(byte *start, int count, int search)
{
	int i;

	for(i = 0; i < count; i++)
		if(start[i] == search)
			return i;
	return -1;
}

/*
=============================================================================

ENGINE FILESYSTEM

=============================================================================
*/

char com_gamedir[MAX_OSPATH];

/*
============
COM_WriteFile

The filename will be prefixed by the current game directory
============
*/
void COM_WriteFile(const char *filename, void *data, int len)
{
	FileHandle_t handle;
	char name[MAX_OSPATH];

	sprintf(name, "%s/%s", com_gamedir, filename);

	handle = FS_Open(name, "wb");
	if(!handle)
	{
		Sys_Printf("COM_WriteFile: failed on %s\n", name);
		return;
	}

	Sys_Printf("COM_WriteFile: %s\n", name);
	FS_Write(data, len, handle);
	FS_Close(handle);
}

/*
============
COM_CreatePath

Only used for CopyFile and download
============
*/
void COM_CreatePath(const char *path)
{
	char *ofs;

	for(ofs = (char *)path + 1; *ofs; ofs++)
	{
		if(*ofs == '/')
		{
			// create the directory
			*ofs = 0;
			FS_mkdir(path);
			*ofs = '/';
		}
	}
}

/*
===========
COM_CopyFile

Copies a file over from the net to the local cache, creating any directories
needed.  This is for the convenience of developers using ISDN from home.
===========
*/
void COM_CopyFile(const char *netpath, char *cachepath)
{
	FileHandle_t in, out;
	int remaining, count;
	char buf[4096];

	in = FS_Open(netpath, "rb");
	remaining = FS_Size(in);
	COM_CreatePath(cachepath); // create directories up to the cache file
	out = FS_Open(cachepath, "wb");

	while(remaining)
	{
		if(remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		FS_Read(buf, count, in);
		FS_Write(buf, count, out);
		remaining -= count;
	}

	FS_Close(in);
	FS_Close(out);
}

/*
============
COM_LoadFile

Filename are relative to the quake directory.
Always appends a 0 byte to the loaded data.
============
*/
cache_user_t *loadcache;
byte *loadbuf;
int loadsize;
// TODO: byte *COM_LoadFile(char *path, int usehunk, int *pLength)
byte *COM_LoadFile(const char *path, int usehunk, int *pLength)
{
	FileHandle_t h;
	byte *buf;
	char base[32];
	int len;

	buf = NULL; // quiet compiler warning

	// look for it in the filesystem or pack files
	h = FS_Open(path, "rb");
	if(!h)
		return NULL;
	
	len = FS_Size(h);
	
	if(pLength)
		*pLength = len;

	// extract the filename base name for hunk tag
	COM_FileBase(path, base);

	if(usehunk == 1)
		buf = Hunk_AllocName(len + 1, base);
	else if(usehunk == 2)
		buf = Hunk_TempAlloc(len + 1);
	else if(usehunk == 0)
		buf = Z_Malloc(len + 1);
	else if(usehunk == 3)
		buf = Cache_Alloc(loadcache, len + 1, base);
	else if(usehunk == 4)
	{
		if(len + 1 > loadsize)
			buf = Hunk_TempAlloc(len + 1);
		else
			buf = loadbuf;
	}
	else
		Sys_Error("COM_LoadFile: bad usehunk");

	if(!buf)
		Sys_Error("COM_LoadFile: not enough space for %s", path);

	((byte *)buf)[len] = 0;

	Draw_BeginDisc();
	FS_Read(buf, len, h);
	FS_Close(h);
	Draw_EndDisc();

	return buf;
}

byte *COM_LoadHunkFile(const char *path)
{
	return COM_LoadFile(path, 1, NULL);
}

byte *COM_LoadTempFile(const char *path)
{
	return COM_LoadFile(path, 2, NULL);
}

void COM_LoadCacheFile(const char *path, struct cache_user_s *cu)
{
	loadcache = cu;
	COM_LoadFile(path, 3, NULL);
}

// uses temp hunk if larger than bufsize
byte *COM_LoadStackFile(const char *path, void *buffer, int bufsize)
{
	byte *buf;

	loadbuf = (byte *)buffer;
	loadsize = bufsize;
	buf = COM_LoadFile(path, 4, NULL);

	return buf;
}

byte *COM_LoadFileForMe(const char *sFileName, int *pLength)
{
	// TODO
	return NULL;
};

char *COM_ParseFile(char *data, char *token)
{
	return NULL; // TODO
};

void COM_FreeFile(void *buffer)
{
	// TODO
};

int COM_CompareFileTime(const char *sFileNameA, const char *sFileNameB, int *pCompare)
{
	// TODO
	return 0;
};

// TODO: temp?
void COM_AddGameDirectory(const char *dir)
{
	FS_AddSearchPath(dir, "PLATFORM");
};

void COM_AddAppDirectoryToSearchPath(const char *asBaseDir, const char *asAppName)
{
	// TODO
};

void COM_GetGameDir(char *sGameDir)
{
	// TODO
};

/*
================
COM_InitFilesystem
================
*/
void COM_InitFilesystem()
{
	int i, j;
	char basedir[MAX_OSPATH];
	//searchpath_t *search;

	//
	// -basedir <path>
	// Overrides the system supplied base directory (under GAMENAME)
	//
	i = COM_CheckParm("-basedir");
	if(i && i < com_argc - 1)
		strcpy(basedir, com_argv[i + 1]);
	else
		strcpy(basedir, host_parms.basedir);

	j = strlen(basedir);

	if(j > 0)
	{
		if((basedir[j - 1] == '\\') || (basedir[j - 1] == '/'))
			basedir[j - 1] = 0;
	}

	//
	// -cachedir <path>
	// Overrides the system supplied cache directory (NULL or /qcache)
	// -cachedir - will disable caching.
	//
	// TODO: unused
	/*
	i = COM_CheckParm("-cachedir");
	if(i && i < com_argc - 1)
	{
		if(com_argv[i + 1][0] == '-')
			com_cachedir[0] = 0;
		else
			strcpy(com_cachedir, com_argv[i + 1]);
	}
	else if(host_parms.cachedir)
		strcpy(com_cachedir, host_parms.cachedir);
	else
		com_cachedir[0] = 0;
	*/

	//
	// start up with GAMENAME by default (valve)
	//
	COM_AddGameDirectory(va("%s/" GAMENAME, basedir));
	strcpy(com_gamedir, GAMENAME); // TODO

	//
	// -game <gamedir>
	// Adds basedir/gamedir as an override game
	//
	i = COM_CheckParm("-game");
	if(i && i < com_argc - 1)
	{
		COM_AddGameDirectory(va("%s/%s", basedir, com_argv[i + 1]));
		strcpy(com_gamedir, com_argv[i + 1]); // TODO
	}

	//
	// -path <dir or packfile> [<dir or packfile>] ...
	// Fully specifies the exact serach path, overriding the generated one
	//
	// TODO: unused?
	/*
	i = COM_CheckParm("-path");
	if(i)
	{
		com_searchpaths = NULL;
		while(++i < com_argc)
		{
			if(!com_argv[i] || com_argv[i][0] == '+' || com_argv[i][0] == '-')
				break;

			search = Hunk_Alloc(sizeof(searchpath_t));
			if(!strcmp(COM_FileExtension(com_argv[i]), "pak"))
			{
				search->pack = COM_LoadPackFile(com_argv[i]);
				if(!search->pack)
					Sys_Error("Couldn't load packfile: %s", com_argv[i]);
			}
			else
				strcpy(search->filename, com_argv[i]);
			search->next = com_searchpaths;
			com_searchpaths = search;
		}
	}
	*/
}
