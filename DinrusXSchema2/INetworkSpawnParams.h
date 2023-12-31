// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

namespace Schematyc2
{
	struct INetworkSpawnParams : public ISerializableInfo
	{
		virtual _smart_ptr<INetworkSpawnParams> Clone() const = 0;
	};

	typedef _smart_ptr<INetworkSpawnParams> INetworkSpawnParamsPtr;

	struct IObjectNetworkSpawnParams : public ISerializableInfo
	{
		virtual void AddComponentSpawnParams(uint32 componentIdx, const INetworkSpawnParamsPtr& pParams) = 0;
		virtual INetworkSpawnParamsPtr GetComponentSpawnParams(uint32 componentIdx) const = 0;
	};

	typedef _smart_ptr<IObjectNetworkSpawnParams> IObjectNetworkSpawnParamsPtr;
}
