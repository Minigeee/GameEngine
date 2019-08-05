#include <Core/StringHash.h>

#include <cstring>

///////////////////////////////////////////////////////////////////////////////

StringHash::StringHash() :
	mValue		(0)
{

}

StringHash::StringHash(const char* str) :
	mValue		(Hash(str))
{

}

StringHash& StringHash::operator=(const char* str)
{
	mValue = Hash(str);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

StringHash::operator Uint32() const
{
	return mValue;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 StringHash::Hash(const char* str)
{
	Uint32 hash = 0;

	while (*str != 0)
		hash = (hash * 0x64CD6DC1) + *str++;

	return hash;
}

///////////////////////////////////////////////////////////////////////////////