#include <Core/Hash.h>

///////////////////////////////////////////////////////////////////////////////

Uint32 Hash32(void* ptr, Uint32 size)
{
	Uint32 hash = 0;
	Uint8* p = (Uint8*)ptr;

	for (Uint32 i = 0; i < size; ++i)
		hash = (hash * 0x64CD6DC1) + *p++;

	return hash;
}

///////////////////////////////////////////////////////////////////////////////

Uint64 Hash64(void* ptr, Uint32 size)
{
	Uint64 hash = 0;
	Uint8* p = (Uint8*)ptr;

	for (Uint32 i = 0; i < size; ++i)
		hash = (hash * 0x64CD6DC1) + *p++;

	return hash;
}

///////////////////////////////////////////////////////////////////////////////