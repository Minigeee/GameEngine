#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

class StringHash
{
public:
	StringHash();
	StringHash(const char* str);
	StringHash& operator=(const char* str);

	operator Uint32() const;

	/* Hash function */
	static Uint32 Hash(const char* str);

private:
	/* Value of hash */
	Uint32 mValue;
};

///////////////////////////////////////////////////////////////////////////////

#endif