#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include <Resource/XmlNode.h>

///////////////////////////////////////////////////////////////////////////////

class XmlDocument : public XmlNode
{
public:
	XmlDocument();
	~XmlDocument();

	/* Load XML document from file */
	bool Load(const char* fname);

private:
	/* RapidXML document */
	void* mDoc;
	/* Text buffer */
	char* mBuffer;
};

///////////////////////////////////////////////////////////////////////////////

#endif