// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Extension/DrxGUID.h>
#include <DinrusX/DinrusXSchema/IString.h>

namespace Schematyc
{
	// Import DrxGUID from global namespace so that Schematyc::DrxGUID is equal to DrxGUID
	using ::DrxGUID;

namespace GUID
{
// Get empty GUID.
// SchematycTODO : Replace with constexpr global variable?
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr DrxGUID Empty()
{
	return DrxGUID(DrxGUID::Null());
}

// Is GUID empty?
////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool IsEmpty(const DrxGUID& guid)
{
	return guid == Empty();
}

// Write GUID to string.
////////////////////////////////////////////////////////////////////////////////////////////////////
inline void ToString(IString& output, const DrxGUID& guid)
{
	char buf[40];
	guid.ToString(buf);
	output.assign(buf);
}


} // GUID
} // Schematyc
