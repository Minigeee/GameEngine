#ifndef XML_NODE_H
#define XML_NODE_H

#include <Resource/XmlAttribute.h>

///////////////////////////////////////////////////////////////////////////////

class XmlNode
{
public:
	virtual ~XmlNode();

	/* Get name */
	char* GetName() const;
	/* Get value */
	char* GetValue() const;

	/* Get first node with name */
	XmlNode GetFirstNode(const char* name = 0) const;
	/* Get last node with name */
	XmlNode GetLastNode(const char* name = 0) const;
	/* Get next sibling with name */
	XmlNode GetNextSibling(const char* name = 0) const;
	/* Get previous sibling with name */
	XmlNode GetPrevSibling(const char* name = 0) const;
	/* Get first attribute with name */
	XmlAttribute GetFirstAttribute(const char* name = 0) const;
	/* Get last attribute with name */
	XmlAttribute GetLastAttribute(const char* name = 0) const;

	/* Get parent */
	XmlNode GetParent() const;

	/* Returns if node ptr exists */
	bool Exists() const;

protected:
	/* Only created from self */
	XmlNode(void* node);

	/* RapidXML Node */
	void* mNode;
};

///////////////////////////////////////////////////////////////////////////////

#endif