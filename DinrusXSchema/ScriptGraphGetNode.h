// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/RuntimeGraph.h>
#include <DinrusX/DinrusXSchema/GUID.h>

#include <DinrusX/DinrusXSchema/ScriptGraphNodeModel.h>

namespace Schematyc
{
class CScriptGraphGetNode : public CScriptGraphNodeModel
{
private:

	struct EOutputIdx
	{
		enum : uint32
		{
			Value = 0
		};
	};

	struct SRuntimeData
	{
		SRuntimeData(uint32 _pos);
		SRuntimeData(const SRuntimeData& rhs);

		static void ReflectType(CTypeDesc<SRuntimeData>& desc);

		uint32       pos;
	};

	struct SComponentPropertyRuntimeData
	{
		static void ReflectType(CTypeDesc<SComponentPropertyRuntimeData>& desc) { desc.SetGUID("8A133E1C-6EF3-4110-859E-7853956045F2"_drx_guid); };

		uint32 componentMemberIndex;
		uint32 componentIdx;
	};

public:

	CScriptGraphGetNode();
	CScriptGraphGetNode(const DrxGUID& referenceGUID,uint32 componentMemberId=0);

	// CScriptGraphNodeModel
	virtual void  Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const override;
	virtual DrxGUID GetTypeGUID() const override;
	virtual void  CreateLayout(CScriptGraphNodeLayout& layout) override;
	virtual void  LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void  Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void  Validate(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void  RemapDependencies(IGUIDRemapper& guidRemapper) override;
	// ~CScriptGraphNodeModel

	static void Register(CScriptGraphNodeFactory& factory);

private:

	static SRuntimeResult Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);
	static SRuntimeResult ExecuteArray(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);
	static SRuntimeResult ExecuteGetComponentProperty(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);

public:

	static const DrxGUID ms_typeGUID;

private:

	DrxGUID m_referenceGUID;
	uint32 m_componentMemberId = 0;
};
}
