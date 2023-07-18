// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/yasli/JSONIArchive.h>
#include <DinrusX/CoreX/Serialization/yasli/JSONOArchive.h>
#include <DinrusX/CoreX/Serialization/yasli/BinArchive.h>
#include <DinrusX/DinrusXSys/XmlIArchive.h>
#include <DinrusX/DinrusXSys/XmlOArchive.h>
#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/CoreX/Serialization/ClassFactory.h>
#include <DinrusX/DinrusXSys/IDrxPak.h>

namespace Serialization
{
constexpr const char* kXmlVersionAttribute = "DrxXmlVersion";
constexpr EDrxXmlVersion kXmlVersionCurrent = EDrxXmlVersion::Version2;

static int g_sys_archive_host_xml_version = (int)kXmlVersionCurrent;


bool LoadFile(std::vector<char>& content, const char* filename, bool bFileCanBeOnDisk)
{
	FILE* f = gEnv->pDrxPak->FOpen(filename, "rb", bFileCanBeOnDisk ? IDrxPak::FOPEN_ONDISK : 0);
	if (!f)
		return false;

	gEnv->pDrxPak->FSeek(f, 0, SEEK_END);
	size_t size = gEnv->pDrxPak->FTell(f);
	gEnv->pDrxPak->FSeek(f, 0, SEEK_SET);

	content.resize(size);
	bool result = true;
	if (size != 0)
		result = gEnv->pDrxPak->FRead(&content[0], size, f) == size;
	gEnv->pDrxPak->FClose(f);
	return result;
}

class CArchiveHost : public IArchiveHost
{
public:
	bool LoadJsonFile(const SStruct& outObj, const char* filename, bool bCanBeOnDisk) override
	{
		std::vector<char> content;
		if (!LoadFile(content, filename, true))
			return false;
		yasli::JSONIArchive ia;
		if (!ia.open(content.data(), content.size()))
			return false;
		return ia(outObj);
	}

	bool SaveJsonFile(const char* gameFilename, const SStruct& obj) override
	{
		char buffer[IDrxPak::g_nMaxPath];
		const char* filename = gEnv->pDrxPak->AdjustFileName(gameFilename, buffer, IDrxPak::FLAGS_FOR_WRITING);
		yasli::JSONOArchive oa;
		if (!oa(obj))
			return false;
		return oa.save(filename);
	}

	bool LoadJsonBuffer(const SStruct& obj, const char* buffer, size_t bufferLength) override
	{
		if (bufferLength == 0)
			return false;
		yasli::JSONIArchive ia;
		if (!ia.open(buffer, bufferLength))
			return false;
		return ia(obj);
	}

	bool SaveJsonBuffer(DynArray<char>& buffer, const SStruct& obj) override
	{
		yasli::JSONOArchive oa;
		if (!oa(obj))
			return false;
		buffer.assign(oa.buffer(), oa.buffer() + oa.length());
		return true;
	}

	bool LoadBinaryFile(const SStruct& obj, const char* filename) override
	{
		std::vector<char> content;
		if (!LoadFile(content, filename, false))
			return false;
		yasli::BinIArchive ia;
		if (!ia.open(content.data(), content.size()))
			return false;
		return ia(obj);
	}

	bool SaveBinaryFile(const char* gameFilename, const SStruct& obj) override
	{
		char buffer[IDrxPak::g_nMaxPath];
		const char* filename = gEnv->pDrxPak->AdjustFileName(gameFilename, buffer, IDrxPak::FLAGS_FOR_WRITING);
		yasli::BinOArchive oa;
		obj(oa);
		return oa.save(filename);
	}

	bool LoadBinaryBuffer(const SStruct& obj, const char* buffer, size_t bufferLength) override
	{
		if (bufferLength == 0)
			return false;
		yasli::BinIArchive ia;
		if (!ia.open(buffer, bufferLength))
			return false;
		return ia(obj);
	}

	bool SaveBinaryBuffer(DynArray<char>& buffer, const SStruct& obj) override
	{
		yasli::BinOArchive oa;
		obj(oa);
		buffer.assign(oa.buffer(), oa.buffer() + oa.length());
		return true;
	}

	bool CloneBinary(const SStruct& dest, const SStruct& src) override
	{
		yasli::BinOArchive oa;
		src(oa);
		yasli::BinIArchive ia;
		if (!ia.open(oa.buffer(), oa.length()))
			return false;
		dest(ia);
		return true;
	}

	bool CompareBinary(const SStruct& lhs, const SStruct& rhs) override
	{
		yasli::BinOArchive oa1;
		lhs(oa1);
		yasli::BinOArchive oa2;
		rhs(oa2);
		if (oa1.length() != oa2.length())
			return false;
		return memcmp(oa1.buffer(), oa2.buffer(), oa1.length()) == 0;
	}

	bool SaveXmlFile(const char* filename, const SStruct& obj, const char* rootNodeName, EDrxXmlVersion forceVersion = EDrxXmlVersion::Auto) override
	{
		XmlNodeRef node = SaveXmlNode(obj, rootNodeName, forceVersion);
		if (!node)
			return false;
		return node->saveToFile(filename);
	}

	bool LoadXmlFile(const SStruct& obj, const char* filename, EDrxXmlVersion forceVersion = EDrxXmlVersion::Auto) override
	{
		XmlNodeRef node = gEnv->pSystem->LoadXmlFromFile(filename);
		if (!node)
			return false;
		return LoadXmlNode(obj, node, forceVersion);
	}

	static EDrxXmlVersion SelectDrxXmlVersionToSave(EDrxXmlVersion forceVersion)
	{
		EDrxXmlVersion version = EDrxXmlVersion::Auto;
		if (forceVersion == EDrxXmlVersion::Auto)
		{
			version = (EDrxXmlVersion)g_sys_archive_host_xml_version;
		}
		if (version == EDrxXmlVersion::Auto || version >= EDrxXmlVersion::Last)
		{
			version = kXmlVersionCurrent;
		}
		return version;
	}

	XmlNodeRef SaveXmlNode(const SStruct& obj, const char* nodeName, EDrxXmlVersion forceVersion = EDrxXmlVersion::Auto) override
	{
		const EDrxXmlVersion version = SelectDrxXmlVersionToSave(forceVersion);
		switch (version)
		{
		case EDrxXmlVersion::Auto:
		case EDrxXmlVersion::Last:
			break;
		case EDrxXmlVersion::Version1: return SaveXmlNodeVer1(obj, nodeName);
		case EDrxXmlVersion::Version2: return SaveXmlNodeVer2(obj, nodeName);
		}
		return XmlNodeRef();
	}

	bool SaveXmlNode(XmlNodeRef& node, const SStruct& obj, EDrxXmlVersion forceVersion = EDrxXmlVersion::Auto) override
	{
		const EDrxXmlVersion version = SelectDrxXmlVersionToSave(forceVersion);
		switch (version)
		{
		case EDrxXmlVersion::Auto:
		case EDrxXmlVersion::Last:
			break;
		case EDrxXmlVersion::Version1: return SaveXmlNodeVer1(node, obj);
		case EDrxXmlVersion::Version2: return SaveXmlNodeVer2(node, obj);
		}
		return false;
	}


	XmlNodeRef SaveXmlNodeVer2(const SStruct& obj, const char* nodeName)
	{
		CXmlOutputArchive oa;
		XmlNodeRef node = gEnv->pSystem->CreateXmlNode(nodeName);
		if (!node)
			return XmlNodeRef();
		node->setAttr(kXmlVersionAttribute, (int)EDrxXmlVersion::Version2);
		oa.SetXmlNode(node);
		if (!obj(oa))
			return XmlNodeRef();
		return oa.GetXmlNode();
	}

	bool SaveXmlNodeVer2(XmlNodeRef& node, const SStruct& obj)
	{
		if (!node)
			return false;
		CXmlOutputArchive oa;
		node->setAttr(kXmlVersionAttribute, (int)EDrxXmlVersion::Version2);
		oa.SetXmlNode(node);
		return obj(oa);
	}

	XmlNodeRef SaveXmlNodeVer1(const SStruct& obj, const char* nodeName)
	{
		CXmlOArchiveVer1 oa;
		XmlNodeRef node = gEnv->pSystem->CreateXmlNode(nodeName);
		if (!node)
			return XmlNodeRef();
		oa.SetXmlNode(node);
		if (!obj(oa))
			return XmlNodeRef();
		return oa.GetXmlNode();
	}

	bool SaveXmlNodeVer1(XmlNodeRef& node, const SStruct& obj)
	{
		if (!node)
			return false;
		CXmlOArchiveVer1 oa;
		oa.SetXmlNode(node);
		return obj(oa);
	}

	static EDrxXmlVersion SelectDrxXmlVersionToLoad(const XmlNodeRef& node, EDrxXmlVersion forceVersion)
	{
		EDrxXmlVersion version = EDrxXmlVersion::Version1;   // Default to old version
		if (forceVersion == EDrxXmlVersion::Auto)
		{
			int iVersion = (int)version;
			node->getAttr(kXmlVersionAttribute, iVersion);
			version = (EDrxXmlVersion)iVersion;
		}
		else
		{
			version = forceVersion;
		}
		return version;
	}

	bool LoadXmlNode(const SStruct& obj, const XmlNodeRef& node, EDrxXmlVersion forceVersion = EDrxXmlVersion::Auto) override
	{
		const EDrxXmlVersion version = SelectDrxXmlVersionToLoad(node, forceVersion);
		switch (version)
		{
		case EDrxXmlVersion::Auto:
		case EDrxXmlVersion::Last:
			break;
		case EDrxXmlVersion::Version1: return LoadXmlNodeVer1(obj, node);
		case EDrxXmlVersion::Version2: return LoadXmlNodeVer2(obj, node);
		}
		return false;
	}

	bool LoadXmlNodeVer1(const SStruct& obj, const XmlNodeRef& node)
	{
		CXmlIArchiveVer1 ia;
		ia.SetXmlNode(node);
		return obj(ia);
	}

	bool LoadXmlNodeVer2(const SStruct& obj, const XmlNodeRef& node)
	{
		CXmlInputArchive ia;
		ia.SetXmlNode(node);
		return obj(ia);
	}

	bool LoadBlackBox(const SStruct& outObj, SBlackBox& box) override
	{
		if (box.format && box.data)
		{
			if (strcmp(box.format, "json") == 0)
			{
				return LoadJsonBuffer(outObj, static_cast<const char*>(box.data), box.size);
			}
			else if (strcmp(box.format, "xml") == 0)
			{
				return LoadXmlNode(outObj, *static_cast<const XmlNodeRef*>(box.data), (EDrxXmlVersion)box.xmlVersion);
			}
		}
		return false;
	}
};

IArchiveHost* CreateArchiveHost()
{
	return new CArchiveHost;
}

void RegisterArchiveHostCVars()
{
	REGISTER_CVAR2("sys_archive_host_xml_version", &g_sys_archive_host_xml_version, g_sys_archive_host_xml_version, VF_NULL,
		"Selects default DrxXmlVersion");
}


}
