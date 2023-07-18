// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   RegFactoryNode.h
//  Version:     v1.00
//  Created:     02/25/2009 by CarstenW
//  Описание: Part of DinrusX's extension framework.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef _REGFACTORYNODE_H_
#define _REGFACTORYNODE_H_

#pragma once

struct IDrxFactory;
struct SRegFactoryNode;

extern SRegFactoryNode* g_pHeadToRegFactories;
extern "C" DLL_EXPORT SRegFactoryNode* GetHeadToRegFactories();
typedef SRegFactoryNode*(*PtrFunc_GetHeadToRegFactories)();

struct SRegFactoryNode
{
	SRegFactoryNode()
	{
	}

	SRegFactoryNode(IDrxFactory* pFactory)
		: m_pFactory(pFactory)
		, m_pNext(g_pHeadToRegFactories)
	{
		g_pHeadToRegFactories = this;
	}

	static void* operator new(size_t, void* p)
	{
		return p;
	}

	static void operator delete(void*, void*)
	{
	}

	IDrxFactory*     m_pFactory;
	SRegFactoryNode* m_pNext;
};

#endif // #ifndef _REGFACTORYNODE_H_
