# Memory Allocation

H_??? The hunk manages the entire memory block given to quake.  It must be
contiguous.  Memory can be allocated from either the low or high end in a
stack fashion.  The only way memory is released is by resetting one of the
pointers.

Hunk allocations should be given a name, so the Hunk_Print () function
can display usage.

Hunk allocations are guaranteed to be 16 byte aligned.

The video buffers are allocated high to avoid leaving a hole underneath
server allocations when changing to a higher video mode.


Z_??? Zone memory functions used for small, dynamic allocations like text
strings from command input.  There is only about 48K for it, allocated at
the very bottom of the hunk.

Cache_??? Cache memory is for objects that can be dynamically loaded and
can usefully stay persistant between levels.  The size of the cache
fluctuates from level to level.

To allocate a cachable object


Temp_??? Temp memory is used for file loading and surface caching.  The size
of the cache memory is adjusted so that there is a minimum of 512k remaining
for temp memory.

>
>------ Top of Memory -------
>
>high hunk allocations
>
><--- high hunk reset point held by vid
>
>video buffer
>
>z buffer
>
>surface cache
>
><--- high hunk used
>
>cachable memory
>
><--- low hunk used
>
>client and server low hunk allocations
>
><-- low hunk reset point held by host
>
>startup hunk allocations
>
>Zone block
>
>----- Bottom of Memory -----
>