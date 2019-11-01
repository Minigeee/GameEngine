#ifndef IMAGE_H
#define IMAGE_H

#include <Core/TypeInfo.h>

#include <Resource/Loadable.h>

///////////////////////////////////////////////////////////////////////////////

class Image : public Loadable
{
	TYPE_INFO(Image);

public:
	enum DataType
	{
		Ubyte	= 0x1401,
		Ushort	= 0x1403,
		Float	= 0x1406
	};

public:
	Image();
	~Image();

	/* Load image from file */
	bool Load(const char* fname, DataType type = Ubyte);

	/* Set image data (Data should be allocated with malloc) */
	void SetData(void* data, Uint32 w, Uint32 h, Uint32 c, DataType type = Ubyte);
	/* Free image data */
	void FreeData();

	/* Set image size */
	void SetSize(Uint32 w, Uint32 h);
	/* Set data type */
	void SetDataType(DataType type);
	/* Set number of color channels */
	void SetNumChannels(Uint32 c);

	/* Get image data */
	void* GetData() const;
	/* Get image width */
	Uint32 GetWidth() const;
	/* Get image height */
	Uint32 GetHeight() const;
	/* Get data type */
	DataType GetDataType() const;
	/* Get number of channels */
	Uint32 GetNumChannels() const;

private:
	/* Image data */
	void* mData;
	/* Image width */
	Uint32 mWidth;
	/* Image height */
	Uint32 mHeight;
	/* Image data type */
	DataType mDataType;
	/* Number of color channels */
	Uint32 mNumChannels;
};

///////////////////////////////////////////////////////////////////////////////

#endif