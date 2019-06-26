#ifndef TYPE_INFO_H
#define TYPE_INFO_H

#include <Core/DataTypes.h>
#include <Core/Macros.h>

///////////////////////////////////////////////////////////////////////////////

/* Keeps track of type IDs */
Uint32 TypeCounter();

///////////////////////////////////////////////////////////////////////////////

#define TYPE_INFO(x) \
public: \
	static Uint32 StaticTypeID() { static Uint32 id = TypeCounter(); return id; } \
	static const char* StaticTypeName() { return STR(x); } \
	Uint32 GetTypeID() const { return StaticTypeID(); } \
	const char* GetTypeName() const { return STR(x); }

///////////////////////////////////////////////////////////////////////////////

#endif