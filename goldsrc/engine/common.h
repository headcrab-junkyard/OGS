/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2005 Id Software, Inc.
 * Copyright (C) 2018-2021, 2023 BlackPhrase
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
/// @brief general definitions

#pragma once

#include <common/const.h>

#define	MAX_SERVERINFO_STRING 512
#define	MAX_LOCALINFO_STRING 32768

//============================================================================

typedef struct usercmd_s usercmd_t; // TODO

typedef struct sizebuf_s
{
	qboolean allowoverflow; // if false, do a Sys_Error
	qboolean overflowed;    // set to true if the buffer size failed
	qboolean oob;           // set to true if the buffer size failed (with allowoverflow set)
	//const char *name; // TODO
	//int flags; // TODO
	byte *data;
	int maxsize;
	int cursize;
	//int readcount;
	int bit;				// for bitwise reads and writes
} sizebuf_t;

void SZ_Alloc(sizebuf_t *buf, int startsize); // TODO: not present in qw
void SZ_Free(sizebuf_t *buf); // TODO: not present in qw
void SZ_Clear(sizebuf_t *buf);
void *SZ_GetSpace(sizebuf_t *buf, int length);
void SZ_Write(sizebuf_t *buf, const void *data, int length);
void SZ_Print(sizebuf_t *buf, const char *data); // strcats onto the sizebuf

//============================================================================

void ClearLink(link_t *l);
void RemoveLink(link_t *l);
void InsertLinkBefore(link_t *l, link_t *before);
void InsertLinkAfter(link_t *l, link_t *after);

// (type *)STRUCT_FROM_LINK(link_t *link, type, member)
// ent = STRUCT_FROM_LINK(link,entity_t,order)
// FIXME: remove this mess!
#define STRUCT_FROM_LINK(l, t, m) ((t *)((byte *)l - (int)&(((t *)0)->m)))

//============================================================================

#ifndef NULL
#define NULL ((void *)0)
#endif

#define Q_MAXCHAR ((char)0x7f)
#define Q_MAXSHORT ((short)0x7fff)
#define Q_MAXINT ((int)0x7fffffff)
#define Q_MAXLONG ((int)0x7fffffff)
#define Q_MAXFLOAT ((int)0x7fffffff)

#define Q_MINCHAR ((char)0x80)
#define Q_MINSHORT ((short)0x8000)
#define Q_MININT ((int)0x80000000)
#define Q_MINLONG ((int)0x80000000)
#define Q_MINFLOAT ((int)0x7fffffff)

//============================================================================

extern qboolean bigendien;

extern short (*BigShort)(short l);
extern short (*LittleShort)(short l);
extern int (*BigLong)(int l);
extern int (*LittleLong)(int l);
extern float (*BigFloat)(float l);
extern float (*LittleFloat)(float l);

//============================================================================

//struct usercmd_s; // TODO: qw
extern struct usercmd_s nullcmd;

void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteByte(sizebuf_t *sb, int c);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void MSG_WriteCoord(sizebuf_t *sb, float f);
void MSG_WriteAngle(sizebuf_t *sb, float f);
//void MSG_WriteAngle16(sizebuf_t *sb, float f); // TODO: qw
//void MSG_WriteDeltaUsercmd(sizebuf_t *sb, struct usercmd_s *from, struct usercmd_s *cmd); // TODO: qw
void MSG_WriteBuf(sizebuf_t *buf, const void *data, int length);

// TODO: not present in qw??
extern int msg_readcount;
extern qboolean msg_badread; // set if a read goes beyond end of message

void MSG_BeginReading();
int MSG_GetReadCount(); // TODO: used by sv_user
int MSG_ReadOneBit();
int MSG_ReadChar();
int MSG_ReadByte();
int MSG_ReadShort();
int MSG_ReadWord();
int MSG_ReadLong();
float MSG_ReadFloat();
char *MSG_ReadString();
char *MSG_ReadStringLine();

float MSG_ReadCoord();
float MSG_ReadAngle();
//float MSG_ReadAngle16(); // TODO: qw
void MSG_ReadDeltaUsercmd(struct usercmd_s *from, struct usercmd_s *cmd); // TODO: used by sv_user and cl_ents

//============================================================================

//
void Q_memset(void *dest, int fill, int count);
void Q_memcpy(void *dest, const void *src, int count);
int Q_memcmp(const void *m1, const void *m2, int count);

void Q_strcpy(char *dest, const char *src);
void Q_strncpy(char *dest, const char *src, int count);
int Q_strlen(const char *str);
const char *Q_strrchr(const char *s, char c);
void Q_strcat(char *dest, const char *src);
int Q_strcmp(const char *s1, const char *s2);
int Q_strncmp(const char *s1, const char *s2, int count);
int Q_strcasecmp(const char *s1, const char *s2);
int Q_strncasecmp(const char *s1, const char *s2, int n);
//
// TODO: qw
/*
#define Q_memset(d, f, c) memset((d), (f), (c))
#define Q_memcpy(d, s, c) memcpy((d), (s), (c))
#define Q_memcmp(m1, m2, c) memcmp((m1), (m2), (c))
#define Q_strcpy(d, s) strcpy((d), (s))
#define Q_strncpy(d, s, n) strncpy((d), (s), (n))
#define Q_strlen(s) ((int)strlen(s))
#define Q_strrchr(s, c) strrchr((s), (c))
#define Q_strcat(d, s) strcat((d), (s))
#define Q_strcmp(s1, s2) strcmp((s1), (s2))
#define Q_strncmp(s1, s2, n) strncmp((s1), (s2), (n))

#ifdef _WIN32

#define Q_strcasecmp(s1, s2) _stricmp((s1), (s2))
#define Q_strncasecmp(s1, s2, n) _strnicmp((s1), (s2), (n))

#else

#define Q_strcasecmp(s1, s2) strcasecmp((s1), (s2))
#define Q_strncasecmp(s1, s2, n) strncasecmp((s1), (s2), (n))

#endif
*/
//

int Q_atoi(const char *str);
float Q_atof(const char *str);

//============================================================================

extern char com_token[1024];
extern qboolean com_eof;

const char *COM_Parse(const char *data);

extern int com_argc;
extern char **com_argv;

int COM_CheckParm(const char *parm);
//void COM_AddParm(const char *parm); // TODO: qw

void COM_Init(const char *path); // TODO: nothing in qw
void COM_InitArgv(int argc, char **argv);

const char *COM_SkipPath(const char *pathname);
int COM_ExpandFilename(const char *asFileName, char *asNameOutBuffer, int anOutBufferSize);
void COM_StripExtension(const char *in, char *out);
char *COM_FileExtension(const char *in);
void COM_FileBase(const char *in, char *out);
void COM_DefaultExtension(char *path, const char *extension);

char *va(const char *format, ...);
// does a varargs printf into a temp buffer

//============================================================================

//extern int com_filesize;
struct cache_user_s;

extern char com_gamedir[MAX_OSPATH];

byte *COM_LoadFile(const char *path, int usehunk, int *pLength); // TODO: was non const-corrent
char *COM_ParseFile(char *data, char *token); // TODO
void COM_FreeFile(void *buffer); // TODO

void COM_WriteFile(const char *filename, void *data, int len);
//int COM_OpenFile(const char *filename, int *hndl); // TODO: not present in qw
//int COM_FOpenFile(const char *filename, FILE **file);
//void COM_CloseFile(int h); // TODO: void COM_CloseFile(FILE *h); in qw

byte *COM_LoadStackFile(const char *path, void *buffer, int bufsize);
byte *COM_LoadTempFile(const char *path);
byte *COM_LoadHunkFile(const char *path);
void COM_LoadCacheFile(const char *path, struct cache_user_s *cu);

byte *COM_LoadFileForMe(const char *sFileName, int *pLength);

int COM_CompareFileTime(const char *sFileNameA, const char *sFileNameB, int *pCompare);

void COM_AddAppDirectoryToSearchPath(const char *asBaseDir, const char *asAppName);

//void COM_CreatePath(const char *path);

void COM_GetGameDir(char *sGameDir);

// TODO: qw
/*
unsigned Com_BlockChecksum(void *buffer, int length);
void Com_BlockFullChecksum(void *buffer, int len, unsigned char *outbuf);
byte COM_BlockSequenceCheckByte(byte *base, int length, int sequence, unsigned mapchecksum);

int build_number();
*/