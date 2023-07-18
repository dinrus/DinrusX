// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptEnum.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
// Forward declare structures.
struct SScriptEnumTypeInfo;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(SScriptEnumTypeInfo)

class CScriptEnum : public CScriptElementBase<IScriptEnum>
{
private:

	typedef std::vector<string> Constants;

public:

	CScriptEnum();
	CScriptEnum(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptEnum
	virtual uint32      GetConstantCount() const override;
	virtual uint32      FindConstant(const char* szConstant) const override;
	virtual const char* GetConstant(uint32 constantIdx) const override;
	// ~IScriptEnum

private:

	Constants                m_constants;
	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
