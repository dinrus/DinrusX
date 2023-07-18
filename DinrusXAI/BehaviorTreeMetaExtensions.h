// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/IBehaviorTree.h>

namespace BehaviorTree
{
class MetaExtensionFactory : public IMetaExtensionFactory
{
public:

	MetaExtensionFactory();

	// IMetaExtensionFactory
	virtual void RegisterMetaExtensionCreator(MetaExtensionCreator metaExtensionCreator) override;
	virtual void CreateMetaExtensions(MetaExtensionTable& metaExtensionTable) override;
	// ~IExtensionFactory

private:

	typedef DynArray<MetaExtensionCreator> MetaExtensionCreatorArray;

	MetaExtensionCreatorArray m_metaExtensionCreators;
};
}
