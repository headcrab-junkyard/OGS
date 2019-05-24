//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Misc string functions.  This is just what we need to compile SDR,
// stripped out from Steam's version that had a bunch of extra stuff in it.
//
//========================================================================//

#ifndef VSTDLIB_STRTOOLS_H
#define VSTDLIB_STRTOOLS_H
#pragma once

#include <ctype.h>
#include <string.h>
#ifdef __MINGW32__
// for __MINGW_PRINTF_FORMAT
#include <stdio.h>
#endif
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

#include <minbase/minbase_identify.h>
#include <minbase/minbase_annotations.h>

template< class T > class CUtlMemory;
template< class T, class A > class CUtlVector;
template < typename K, typename T, typename I, typename L > class CUtlMap;
class CUtlBuffer;

#ifdef _WIN64
#define str_size unsigned int
#else
#define str_size size_t
#endif

inline int Q_strlen(const char *str) { return static_cast<int>( strlen ( str ) ); }
inline int Q_strcmp(const char *s1, const char *s2) { return strcmp( s1, s2 ); }
inline int Q_memcmp (const void *m1, const void *m2, size_t count){ return memcmp( m1, m2, count ); } 
inline void Q_memmove (OUT_BYTECAP(count) void *dest, IN_BYTECAP(count) const void *src, size_t count)	{ memmove( dest, src, count ); }
inline void Q_memset (OUT_BYTECAP(count) void *dest, int fill, size_t count) { memset( dest, fill, count ); }
inline void Q_memcpy( OUT_BYTECAP( count ) void *dest, IN_BYTECAP( count ) const void *src, size_t count )	{ memcpy( dest, src, count ); }
inline const char*Q_strstr( const char *s1, const char *search ) { return strstr( s1, search ); }
inline char *Q_strchr( char *s, char c ) { return strchr( s, c ); }
inline char*Q_strstr( char *s1, const char *search ) { return (char*)strstr( s1, search ); }

extern int	Q_strncmp( const char *s1, const char *s2, int count );
extern const char*Q_strnistr( const char* pStr, const char* pSearch, int n );
extern const char*Q_strnchr( const char* pStr, char c, int n );
extern int Q_strnicmp( const char *s1, const char *s2, int n );
inline int Q_stricmp( const char *s1, const char *s2 ) { return Q_strnicmp( s1, s2, INT_MAX ); }
const char* Q_stristr( const char* pStr, const char* pSearch );

extern void Q_strncpy( OUT_Z_CAP(maxLen) char *pDest, const char *pSrc, size_t maxLen );
template <size_t maxLenInChars> void Q_strcpy_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], const char *pSrc ) 
{ 
	Q_strncpy( pDest, pSrc, maxLenInChars ); 
}

extern int Q_vsnprintf( OUT_Z_CAP(maxLen) char *pDest, int maxLen, const char *pFormat, va_list params );
extern int Q_vsnprintfRet( OUT_Z_CAP(maxLen) char *pDest, int maxLen, const char *pFormat, va_list params, bool *pbTruncated );
extern int Q_snprintf( OUT_Z_CAP(destLen) char *pDest, size_t destLen, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );
template <size_t maxLenInChars> int Q_sprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int result = Q_vsnprintf( pDest, maxLenInChars, pFormat, params );
	va_end( params );
	return result;
}

template <size_t maxLenInChars> int Q_vsprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, va_list params ) { return Q_vsnprintf( pDest, maxLenInChars, pFormat, params ); }

#define COPY_ALL_CHARACTERS -1
char *Q_strncat(INOUT_Z_CAP(destBufferSize) char *, const char *, size_t destBufferSize, int max_chars_to_copy=COPY_ALL_CHARACTERS );
template <size_t cchDest> char *Q_strcat_safe( INOUT_Z_ARRAY char (&pDest)[cchDest], const char *pSrc, int nMaxCharsToCopy=COPY_ALL_CHARACTERS )
{ 
	return Q_strncat( pDest, pSrc, cchDest, nMaxCharsToCopy ); 
}

// is* helpers
inline bool Q_isspace(char c) { return isspace((unsigned char)c) != 0; }

// Split the specified string on the specified separator.
// Returns a list of strings separated by pSeparator.
// You are responsible for freeing the contents of outStrings (call outStrings.PurgeAndDeleteElements).
extern void Q_SplitString( const char *pString, const char *pSeparator, CUtlVector<char*, CUtlMemory<char*> > &outStrings, bool bIncludeEmptyStrings = false );

// Strips trailing *ASCII* whitespace characters.  (Any
// character that returns true for Q_isspace returns true.)  Doesn't
// handle all unicode whitespace characters
extern void Q_StripTrailingWhitespaceASCII( char *pch );

// trim whitespace from both ends of the string
extern int Q_StrTrim( char *pStr );

#ifdef POSIX
#ifdef ANDROID
#include <wchar.h>
#endif
#define _atoi64 atoll
#define _wtoi(arg) wcstol(arg, NULL, 10)
#ifdef ANDROID
// TODO - Android doesn't support wcstoi64, so just use a basic implementation of our own.
#define _wcstoi64 vstdlib_wcstoi64
#define _wcstoui64 vstdlib_wcstoui64
#else
#define _wcstoi64 wcstoll
#define _wcstoui64 wcstoull
#endif
#define _wtoi64(arg) _wcstoi64(arg, NULL, 10)
#define _i64toa( num, buf, base ) sprintf( buf, "%lld", num )
#define _stricmp strcasecmp
#define _strtoi64 strtoll
#define _strtoui64 strtoull
#define _vsnprintf vsnprintf
#if defined(OSX) || defined(ANDROID)
// TODO - OSX doesn't support wcscasecmp until 10.7, so just
// use a basic implementation of our own.
#define _wcsicmp vstdlib_wcsicmp
#define _wcsnicmp vstdlib_wcsnicmp
#else
#define _wcsicmp wcscasecmp
#define _wcsnicmp wcsncasecmp
#endif
#define _wcschr wcschr
#ifdef TEXT
#undef TEXT
#endif
#define TEXT(str) str

#endif // POSIX


#endif	// VSTDLIB_STRTOOLS_H
