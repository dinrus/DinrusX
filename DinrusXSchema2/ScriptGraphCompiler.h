// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Rename CGraphCompiler?

#pragma once

#include <DinrusX/DinrusXSchema2/IRuntime.h>
#include <DinrusX/DinrusXSchema2/IScriptGraphNodeCompiler.h>

namespace Schematyc2
{
	class  CLibClass;
	struct IScriptGraphExtension;
	struct IScriptGraphNode;
	struct SCompiledFunction;
	struct SScriptGraphCompilerContext;

	class CScriptGraphCompiler
	{
	public:

		void CompileGraph(CLibClass& libClass, const IScriptGraphExtension& graph, CRuntimeFunction& runtimeFunction);

	private:

		void CompileFunction(CLibClass& libClass, const IScriptGraphExtension& graph, CRuntimeFunction& runtimeFunction, const IScriptGraphNode& graphNode, size_t outputIdx);
		uint32 CompileFunctionForwardsRecursive(SScriptGraphCompilerContext& context, const IScriptGraphNode& graphNode, const SRuntimeActivationParams& activationParams);
		uint32 CompileFunctionBackwardsRecursive(SScriptGraphCompilerContext& context, const IScriptGraphNode& graphNode, uint32 outPos);
		uint32 CompileNode(SScriptGraphCompilerContext& context, const IScriptGraphNode& graphNode);
		void OptimizeFunction(SCompiledFunction& compiledFunction);
		void IndexFunctionRecursive(SCompiledFunction& compiledFunction, std::vector<uint32>& indices, uint32 pos);
		void FinalizeFunction(const SCompiledFunction& compiledFunction, CRuntimeFunction& runtimeFunction);
	};
}
