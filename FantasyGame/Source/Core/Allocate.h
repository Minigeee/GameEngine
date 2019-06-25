#ifndef ALLOCATE_H
#define ALLOCATE_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

/* Aligned memory allocation */
void* Alloc(Uint32 size, Uint32 align = 4);
/* Free allocated memory */
void Free(void* ptr);

///////////////////////////////////////////////////////////////////////////////

#endif