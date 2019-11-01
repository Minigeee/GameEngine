#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef signed	 char		Int8;
typedef unsigned char		Uint8;

typedef signed	 short		Int16;
typedef unsigned short		Uint16;

typedef signed	 int		Int32;
typedef unsigned int		Uint32;

#if defined(_MSC_VER)
typedef signed   __int64	Int64;
typedef unsigned __int64	Uint64;
#else
typedef signed   long long	Int64;
typedef unsigned long long	Uint64;
#endif

#endif