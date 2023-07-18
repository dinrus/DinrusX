// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/FundamentalTypes.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IScriptEnum;

// Script enumeration value.
////////////////////////////////////////////////////////////////////////////////////////////////////
class CScriptEnumValue
{
public:

	CScriptEnumValue(const IScriptEnum* pEnum);
	CScriptEnumValue(const CScriptEnumValue& rhs);

	bool        Serialize(Serialization::IArchive& archive, const char* szName, const char* szLabel);
	void        ToString(IString& output) const;

	static void ReflectType(CTypeDesc<CScriptEnumValue>& desc);

private:

	const IScriptEnum* m_pEnum;       // #SchematycTODO : Wouldn't it be safer to reference by GUID?
	uint32             m_constantIdx; // #SchematycTODO : Wouldn't it be safer to store a string?
};

bool Serialize(Serialization::IArchive& archive, CScriptEnumValue& value, const char* szName, const char* szLabel);

} // Schematyc
