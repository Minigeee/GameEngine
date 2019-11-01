#ifndef LOADABLE_H
#define LOADABLE_H

#include <Core/StringHash.h>

///////////////////////////////////////////////////////////////////////////////

class Loadable
{
public:
	/* Get string hash of file name */
	StringHash GetFileHash() const;

protected:
	/* File name hash */
	StringHash mFileHash;
};

///////////////////////////////////////////////////////////////////////////////

#endif