// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/RuntimeGraph.h>
#include <DinrusX/DinrusXSchema/GUID.h>

#include <DinrusX/DinrusXSchema/ScriptGraphNodeModel.h>

namespace Schematyc
{
class CScriptGraphForNode : public CScriptGraphNodeModel
{
private:

	struct EInputIdx
	{
		enum : uint32
		{
			In = 0,
			Begin,
			End,
			Break
		};
	};

	struct EOutputIdx
	{
		enum : uint32
		{
			Out = 0,
			Loop,
			Pos
		};
	};

	struct SRuntimeData
	{
		SRuntimeData();
		SRuntimeData(const SRuntimeData& rhs);

		static void ReflectType(CTypeDesc<SRuntimeData>& desc);

		int32        pos;
		bool         bBreak;
	};

public:

	CScriptGraphForNode();

	// CScriptGraphNodeModel
	virtual DrxGUID GetTypeGUID() const override;
	virtual void  CreateLayout(CScriptGraphNodeLayout& layout) override;
	virtual void  Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const override;
	// ~CScriptGraphNodeModel

	static void Register(CScriptGraphNodeFactory& factory);

private:

	static SRuntimeResult Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);

public:

	static const DrxGUID ms_typeGUID;
};
} // Schematyc
