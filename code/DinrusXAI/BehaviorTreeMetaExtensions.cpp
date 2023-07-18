// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>

#include <DinrusX/DinrusXAI/BehaviorTreeMetaExtensions.h>

#include <DinrusX/DinrusXAI/IBehaviorTree.h>

namespace BehaviorTree
{

MetaExtensionFactory::MetaExtensionFactory()
	: m_metaExtensionCreators()
{
}

void MetaExtensionFactory::RegisterMetaExtensionCreator(MetaExtensionCreator metaExtensionCreator)
{
	m_metaExtensionCreators.push_back(metaExtensionCreator);
}

void MetaExtensionFactory::CreateMetaExtensions(MetaExtensionTable& metaExtensionTable)
{
	for (MetaExtensionCreator& metaExtensionCreator : m_metaExtensionCreators)
	{
		metaExtensionTable.AddMetaExtension(metaExtensionCreator());
	}
}

}
