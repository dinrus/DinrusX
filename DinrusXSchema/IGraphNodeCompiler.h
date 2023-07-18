// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/RuntimeGraph.h>
#include <DinrusX/DinrusXSchema/Any.h>

namespace Schematyc
{
typedef SRuntimeResult (* RuntimeGraphNodeCallbackPtr)(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);

struct IGraphNodeCompiler
{
	virtual ~IGraphNodeCompiler() {}

	virtual uint32 GetGraphIdx() const = 0;
	virtual uint32 GetGraphNodeIdx() const = 0;

	virtual void   BindCallback(RuntimeGraphNodeCallbackPtr pCallback) = 0;
	virtual void   BindData(const CAnyConstRef& value) = 0;
};
} // Schematyc
