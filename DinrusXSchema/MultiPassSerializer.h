// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{
class CMultiPassSerializer // #SchematycTODO : Templatize this class and use SFINAE to determine which functions are available?
{
public:

	void Serialize(Serialization::IArchive& archive)
	{
		ISerializationContext* pSerializationContext = SerializationContext::Get(archive);
		SCHEMATYC_CORE_ASSERT(pSerializationContext);
		if(pSerializationContext)
		{
			switch (pSerializationContext->GetPass())
			{
			case ESerializationPass::Undefined:
				break;
			case ESerializationPass::LoadDependencies:
				{
					LoadDependencies(archive, *pSerializationContext);
					break;
				}
			case ESerializationPass::Load:
				{
					Load(archive, *pSerializationContext);
					break;
				}
			case ESerializationPass::PostLoad:
				{
					PostLoad(archive, *pSerializationContext);
					break;
				}
			case ESerializationPass::Save:
				{
					Save(archive, *pSerializationContext);
					break;
				}
			case ESerializationPass::Edit:
				{
					Edit(archive, *pSerializationContext);
					break;
				}
			case ESerializationPass::Validate:
				{
					Validate(archive, *pSerializationContext);
					break;
				}
			}
		}
	}

protected:

	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) {}
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) {}
	virtual void PostLoad(Serialization::IArchive& archive, const ISerializationContext& context) {}
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) {}
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) {}
	virtual void Validate(Serialization::IArchive& archive, const ISerializationContext& context) {}
};
} // Schematyc