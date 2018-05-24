

/// @file
/// @brief general asm header file

#pragma once

#ifdef _WIN32
#define __i386__	1
#endif

#ifdef	__i386__
#define id386	1
#else
#define id386	0
#endif
