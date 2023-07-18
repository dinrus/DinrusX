// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/GUIDRemapper.h>

namespace Schematyc
{
bool CGUIDRemapper::Empty() const
{
	return m_guids.empty();
}

void CGUIDRemapper::Bind(const DrxGUID& from, const DrxGUID& to)
{
	m_guids.insert(GUIDs::value_type(from, to));
}

DrxGUID CGUIDRemapper::Remap(const DrxGUID& from) const
{
	GUIDs::const_iterator itGUID = m_guids.find(from);
	return itGUID != m_guids.end() ? itGUID->second : from;
}
} // Schematyc
