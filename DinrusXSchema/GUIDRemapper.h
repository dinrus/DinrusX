// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

namespace Schematyc
{
class CGUIDRemapper : public IGUIDRemapper
{
private:

	typedef std::unordered_map<DrxGUID, DrxGUID> GUIDs;

public:

	// IGUIDRemapper
	virtual bool  Empty() const override;
	virtual void  Bind(const DrxGUID& from, const DrxGUID& to) override;
	virtual DrxGUID Remap(const DrxGUID& from) const override;
	// ~IGUIDRemapper

private:

	GUIDs m_guids;
};
} // Schematyc
