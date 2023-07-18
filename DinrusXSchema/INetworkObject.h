// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/ScopedConnection.h>

namespace Schematyc
{
typedef std::function<void (TSerialize, int32, uint8, int)> NetworkSerializeCallback;

struct INetworkObject
{
	~INetworkObject() {}

	virtual bool   RegisterSerializeCallback(int32 aspects, const NetworkSerializeCallback& callback, CConnectionScope& connectionScope) = 0;
	virtual void   MarkAspectsDirty(int32 aspects) = 0;
	virtual uint16 GetChannelId() const = 0;
	virtual bool   ServerAuthority() const = 0;
	virtual bool   ClientAuthority() const = 0;
	virtual bool   LocalAuthority() const = 0;
};
} // Schematyc
