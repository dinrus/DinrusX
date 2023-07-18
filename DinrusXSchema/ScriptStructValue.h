// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IScriptStruct;
// Forward declare classes.
class CAnyValue;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CAnyValue)

// Script structure value.
////////////////////////////////////////////////////////////////////////////////////////////////////
class CScriptStructValue
{
private:

	typedef std::map<string, CAnyValuePtr> FieldMap;   // #SchematycTODO : Replace map with vector to preserve order!

public:

	CScriptStructValue(const IScriptStruct* pStruct);
	CScriptStructValue(const CScriptStructValue& rhs);

	void        Serialize(Serialization::IArchive& archive);

	static void ReflectType(CTypeDesc<CScriptStructValue>& desc);

private:

	void Refresh();

private:

	const IScriptStruct* m_pStruct;   // #SchematycTODO : Wouldn't it be safer to reference by GUID?
	FieldMap             m_fields;
};

} // Schematyc
