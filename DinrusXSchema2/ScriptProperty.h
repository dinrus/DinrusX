// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/AggregateTypeId.h>
#include <DinrusX/DinrusXSchema2/IScriptFile.h>

#include <DinrusX/DinrusXSchema2/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema2/ScriptVariableDeclaration.h>

namespace Schematyc2
{
	class CScriptProperty : public CScriptElementBase<IScriptProperty>
	{
	public:

		CScriptProperty(IScriptFile& file);
		CScriptProperty(IScriptFile& file, const SGUID& guid, const SGUID& scopeGUID, const char* szName, const SGUID& refGUID, const CAggregateTypeId& typeId);

		// IScriptElement
		virtual EAccessor GetAccessor() const override;
		virtual SGUID GetGUID() const override;
		virtual SGUID GetScopeGUID() const override;
		virtual bool SetName(const char* szName) override;
		virtual const char* GetName() const override;
		virtual void EnumerateDependencies(const ScriptDependancyEnumerator& enumerator) const override;
		virtual void Refresh(const SScriptRefreshParams& params) override;
		virtual void Serialize(Serialization::IArchive& archive) override;
		virtual void RemapGUIDs(IGUIDRemapper& guidRemapper) override;
		// ~IScriptElement

		// IScriptProperty
		virtual SGUID GetRefGUID() const override;
		virtual EOverridePolicy GetOverridePolicy() const override;
		virtual CAggregateTypeId GetTypeId() const override;
		virtual IAnyConstPtr GetValue() const override;
		// ~IScriptProperty

	private:

		SGUID                      m_guid;
		SGUID                      m_scopeGUID;
		SGUID                      m_refGUID;
		EOverridePolicy            m_overridePolicy;
		CScriptVariableDeclaration m_declaration;
	};

	DECLARE_SHARED_POINTERS(CScriptProperty)
}
