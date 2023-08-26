// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>
#include <DinrusX/DinrusXEntity/IEntityComponent.h>

namespace Schematyc
{

struct IEnvComponent : public IEnvElementBase<EEnvElementType::Component>
{
	virtual ~IEnvComponent() {}

	virtual const CEntityComponentClassDesc&  GetDesc() const = 0;
	virtual std::shared_ptr<IEntityComponent> CreateFromPool() const = 0;
	virtual size_t                            GetSize() const = 0;
	virtual std::shared_ptr<IEntityComponent> CreateFromBuffer(void* pBuffer) const = 0;
};

} // Schematyc
