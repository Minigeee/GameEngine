#ifndef XML_ATTRIBUTE_H
#define XML_ATTRIBUTE_H

///////////////////////////////////////////////////////////////////////////////

class XmlNode;

class XmlAttribute
{
	friend XmlNode;

public:
	~XmlAttribute();

	/* Get name */
	const char* GetName() const;
	/* Get value */
	const char* GetValue() const;

	/* Get next attribute with name */
	XmlAttribute GetNextAttribute(const char* name = 0) const;
	/* Get previous attribute with name */
	XmlAttribute GetPrevAttribute(const char* name = 0) const;

	/* Returns if attribute ptr exists */
	bool Exists() const;

private:
	XmlAttribute(void* attr);

	/* RapidXML Attribute */
	void* mAttribute;
};

///////////////////////////////////////////////////////////////////////////////

#endif