#include <Core/Allocate.h>

#include <cstdlib>

///////////////////////////////////////////////////////////////////////////////

void* Alloc(Uint32 size, Uint32 align)
{
	// Max offset of memory
	Uint32 offset = sizeof(void*) + align - 1;

	void* ptr = malloc(size + offset);
	// Calculate start of usable memory
	void** start = (void**)(((Uint64)ptr + offset) & ~(align - 1));
	// Mark start of allocated memory
	start[-1] = ptr;

	return start;
}

///////////////////////////////////////////////////////////////////////////////

void Free(void* ptr)
{
	if (!ptr) return;

	// Free start of allocated memory
	free(((void**)ptr)[-1]);
}

///////////////////////////////////////////////////////////////////////////////