// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
//////////////////////////////////////////////////////////////////////

#ifndef __OPTICSMANAGER_H__
#define __OPTICSMANAGER_H__

#include <DinrusX/CoreX/Renderer/IFlares.h>

class COpticsUpr : public DinrusX3dEngBase, public IOpticsUpr
{
public:

	~COpticsUpr(){}

	void                Reset();

	IOpticsElementBase* Create(EFlareType type) const;
	IOpticsElementBase* GetOptics(int nIndex);

	bool                Load(const char* fullFlareName, int& nOutIndex);
	bool                Load(XmlNodeRef& rootNode, int& nOutIndex);
	bool                AddOptics(IOpticsElementBase* pOptics, const char* name, int& nOutNewIndex);
	bool                Rename(const char* fullFlareName, const char* newFullFlareName);

	void                GetMemoryUsage(IDrxSizer* pSizer) const;
	void                Invalidate();

private:
	IOpticsElementBase* ParseOpticsRecursively(IOpticsElementBase* pParentOptics, XmlNodeRef& node) const;
	EFlareType          GetFlareType(const char* typeStr) const;
	int                 FindOpticsIndex(const char* fullFlareName) const;

private:
	std::vector<IOpticsElementBasePtr> m_OpticsList;
	std::map<string, int>              m_OpticsMap;
	// All flare list which has already been searched for.
	std::set<string>                   m_SearchedOpticsSet;
};

#endif //__OPTICSMANAGER_H__
