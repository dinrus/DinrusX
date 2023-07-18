// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   XMLBinaryWriter.h
//  Created:     21/04/2006 by Michael Smith.
//  Описание:
// -------------------------------------------------------------------------
//  History:
//     8/1/2008 - Modified by Timur
////////////////////////////////////////////////////////////////////////////

#ifndef __XMLBINARYWRITER_H__
#define __XMLBINARYWRITER_H__

#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXSys/XMLBinaryHeaders.h>
#include <vector>
#include <map>

class IXMLDataSink;

namespace XMLBinary
{
class CXMLBinaryWriter
{
public:
	CXMLBinaryWriter();
	bool WriteNode(IDataWriter* pFile, XmlNodeRef node, bool bNeedSwapEndian, XMLBinary::IFilter* pFilter, string& error);

private:
	bool CompileTables(XmlNodeRef node, XMLBinary::IFilter* pFilter, string& error);

	bool CompileTablesForNode(XmlNodeRef node, int nParentIndex, XMLBinary::IFilter* pFilter, string& error);
	bool CompileChildTable(XmlNodeRef node, XMLBinary::IFilter* pFilter, string& error);
	int  AddString(const XmlString& sString);

private:
	// tables.
	typedef std::map<IXmlNode*, int> NodesMap;
	typedef std::map<string, uint>   StringMap;

	std::vector<Node>      m_nodes;
	NodesMap               m_nodesMap;
	std::vector<Attribute> m_attributes;
	std::vector<NodeIndex> m_childs;
	std::vector<string>    m_strings;
	StringMap              m_stringMap;

	uint                   m_nStringDataSize;
};
}

#endif //__XMLBINARYWRITER_H__
