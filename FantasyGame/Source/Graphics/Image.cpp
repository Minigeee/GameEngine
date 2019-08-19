#include <Graphics/Image.h>

#include <Core/LogFile.h>
#include <Resource/StbImage.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Image::Image() :
	mData			(0),
	mWidth			(0),
	mHeight			(0),
	mDataType		(Ubyte),
	mNumChannels	(0)
{

}

Image::~Image()
{
	FreeData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Image::Load(const char* fname, DataType type)
{
	mFileHash = fname;

	int x = 0, y = 0, c = 0;
	void* data = 0;

	// Load image based on type
	if (type == Ubyte)
		data = stbi_load(fname, &x, &y, &c, 0);
	else if (type == Ushort)
		data = stbi_load_16(fname, &x, &y, &c, 0);
	else if (type == Float)
		data = stbi_loadf(fname, &x, &y, &c, 0);

	if (!data)
	{
		LOG_ERROR << "Failed to load image " << fname << "\n";
		return false;
	}

	// Set image params
	mData = data;
	mWidth = x;
	mHeight = y;
	mNumChannels = c;
	mDataType = type;

	LOG_INFO << "Loaded image " << fname << "\n";
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void Image::SetData(void* data, Uint32 w, Uint32 h, Uint32 c, DataType type)
{
	mData = data;
	mWidth = w;
	mHeight = h;
	mNumChannels = c;
	mDataType = type;
}

///////////////////////////////////////////////////////////////////////////////

void Image::FreeData()
{
	if (mData)
		free(mData);
	mData = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Image::SetSize(Uint32 w, Uint32 h)
{
	mWidth = w;
	mHeight = h;
}

void Image::SetDataType(DataType type)
{
	mDataType = type;
}

void Image::SetNumChannels(Uint32 c)
{
	mNumChannels = c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* Image::GetData() const
{
	return mData;
}

Uint32 Image::GetWidth() const
{
	return mWidth;
}

Uint32 Image::GetHeight() const
{
	return mHeight;
}

Image::DataType Image::GetDataType() const
{
	return mDataType;
}

Uint32 Image::GetNumChannels() const
{
	return mNumChannels;
}

///////////////////////////////////////////////////////////////////////////////