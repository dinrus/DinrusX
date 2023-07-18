// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SKELETON_MANAGER__H__
#define __SKELETON_MANAGER__H__

#include <set>
#include "SkeletonLoader.h"

struct IPakSystem;
class IDrxXML;
struct IResourceCompiler;

class SkeletonUpr
{
public:
	SkeletonUpr(IPakSystem* pPakSystem, IDrxXML* pXMLParser, IResourceCompiler* pRc);

	bool LoadSkeletonList(const string& filename, const string& rootPath, const std::set<string>& skeletons);
	const CSkeletonInfo* FindSkeleton(const string& name) const;
	const CSkeletonInfo* LoadSkeleton(const string& name);

private:
	const CSkeletonInfo* LoadSkeletonInfo(const string& name, const string& file);

private:
	IPakSystem* m_pPakSystem;
	IDrxXML* m_pXmlParser;

	string m_rootPath;
	string m_tmpPath;

	typedef std::map<string, string> TNameToFileMap;
	TNameToFileMap m_nameToFile;

	typedef std::map<string, SkeletonLoader> TNameToSkeletonMap;
	TNameToSkeletonMap m_nameToSkeletonInfo;
};

#endif
