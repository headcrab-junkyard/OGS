#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "../linux/glob.h"

#include "../qcommon/qcommon.h"

//===============================================================================

byte *membase;
int maxhunksize;
int curhunksize;

void *Hunk_Begin (int maxsize)
{
	// reserve a huge chunk of memory, but don't commit any yet
	maxhunksize = maxsize + sizeof(int);
	curhunksize = 0;
	membase = mmap(0, maxhunksize, PROT_READ|PROT_WRITE, 
		MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (membase == NULL || membase == (byte *)-1)
		Sys_Error("unable to virtual allocate %d bytes", maxsize);

	*((int *)membase) = curhunksize;

	return membase + sizeof(int);
}

void *Hunk_Alloc (int size)
{
	byte *buf;

	// round to cacheline
	size = (size+31)&~31;
	if (curhunksize + size > maxhunksize)
		Sys_Error("Hunk_Alloc overflow");
	buf = membase + sizeof(int) + curhunksize;
	curhunksize += size;
	return buf;
}

int Hunk_End (void)
{
	byte *n;

	n = mremap(membase, maxhunksize, curhunksize + sizeof(int), 0);
	if (n != membase)
		Sys_Error("Hunk_End:  Could not remap virtual block (%d)", errno);
	*((int *)membase) = curhunksize + sizeof(int);
	
	return curhunksize;
}

void Hunk_Free (void *base)
{
	byte *m;

	if (base) {
		m = ((byte *)base) - sizeof(int);
		if (munmap(m, *((int *)m)))
			Sys_Error("Hunk_Free: munmap failed (%d)", errno);
	}
}

//===============================================================================


/*
================
Sys_Milliseconds
================
*/
int curtime;
int Sys_Milliseconds (void)
{
	struct timeval tp;
	struct timezone tzp;
	static int		secbase;

	gettimeofday(&tp, &tzp);
	
	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;
	
	return curtime;
}

char *strlwr (char *s)
{
	while (*s) {
		*s = tolower(*s);
		s++;
	}
}