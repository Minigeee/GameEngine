#include <Resource/XmlNode.h>

#include <rapidxml/rapidxml.hpp>

#define XML_CAST(x) static_cast<rapidxml::xml_node<>*>(x)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

XmlNode::XmlNode(void* node) :
	mNode		(node)
{

}

XmlNode::~XmlNode()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

char* XmlNode::GetName() const
{
	return XML_CAST(mNode)->name();
}

char* XmlNode::GetValue() const
{
	return XML_CAST(mNode)->value();
}

///////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::GetFirstNode(const char* name) const
{
	return XmlNode(XML_CAST(mNode)->first_node(name));
}

XmlNode XmlNode::GetLastNode(const char* name) const
{
	return XmlNode(XML_CAST(mNode)->last_node(name));
}

XmlNode XmlNode::GetNextSibling(const char* name) const
{
	return XmlNode(XML_CAST(mNode)->next_sibling(name));
}

XmlNode XmlNode::GetPrevSibling(const char* name) const
{
	return XmlNode(XML_CAST(mNode)->previous_sibling(name));
}

XmlAttribute XmlNode::GetFirstAttribute(const char* name) const
{
	return XmlAttribute(XML_CAST(mNode)->first_attribute(name));
}

XmlAttribute XmlNode::GetLastAttribute(const char* name) const
{
	return XmlAttribute(XML_CAST(mNode)->last_attribute(name));
}

///////////////////////////////////////////////////////////////////////////////

XmlNode XmlNode::GetParent() const
{
	return XmlNode(XML_CAST(mNode)->parent());
}

///////////////////////////////////////////////////////////////////////////////

bool XmlNode::Exists() const
{
	return (bool)mNode;
}

///////////////////////////////////////////////////////////////////////////////