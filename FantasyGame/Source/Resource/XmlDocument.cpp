#include <Resource/XmlDocument.h>

#include <Core/DataTypes.h>

#include <rapidxml/rapidxml.hpp>

#include <fstream>

#define XML_CAST(x) static_cast<rapidxml::xml_document<>*>(x)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

XmlDocument::XmlDocument() :
	XmlNode			(0)
{
	mDoc = new rapidxml::xml_document<>;
}

XmlDocument::~XmlDocument()
{
	delete XML_CAST(mDoc);
	free(mBuffer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool XmlDocument::Load(const char* fname)
{
	std::ifstream file(fname, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return false;

	// Get file size
	Uint32 fsize = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = (char*)malloc(fsize + 1);
	file.read(buffer, fsize);
	buffer[fsize] = 0;

	file.close();

	// Parse
	XML_CAST(mDoc)->parse<0>(buffer);

	mNode = XML_CAST(mDoc);
	mBuffer = buffer;
}

///////////////////////////////////////////////////////////////////////////////