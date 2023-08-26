// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXEntity/stdafx.h>
#include <DinrusX/DinrusXEntity/EntityComponentsVector.h>
#include <DinrusX/DinrusXEntity/Entity.h>

SEntityComponentRecord::~SEntityComponentRecord()
{
	Shutdown();
}

void SEntityComponentRecord::Shutdown()
{
	if (pComponent != nullptr)
	{
		static_cast<CEntity*>(pComponent->GetEntity())->ShutDownComponent(*this);
	}
}