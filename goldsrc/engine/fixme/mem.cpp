/// @file

#include "quakedef.h"
#include "mem.h"

void *Mem_Calloc(int number, size_t size)
{
	return calloc((int)number, size);
};