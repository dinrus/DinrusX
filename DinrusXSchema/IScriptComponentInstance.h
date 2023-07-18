// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>
#include <DinrusX/DinrusXSchema/PreprocessorUtils.h>

#include <DinrusX/CoreX/Math/Transform.h>

namespace Schematyc
{
// Forward declare classes.
class CClassProperties;

enum class EScriptComponentInstanceFlags
{
	None     = 0,
	EnvClass = BIT(0)   // Component is part of environment class and can't be removed.
};

typedef CEnumFlags<EScriptComponentInstanceFlags> ScriptComponentInstanceFlags;

struct IScriptComponentInstance : public IScriptElementBase<EScriptElementType::ComponentInstance>
{
	virtual ~IScriptComponentInstance() {}

	virtual DrxGUID                      GetTypeGUID() const = 0;
	virtual ScriptComponentInstanceFlags GetComponentInstanceFlags() const = 0;
	virtual bool                         HasTransform() const = 0;
	
	virtual void                               SetTransform(const DrxTransform::CTransformPtr& transform) = 0;
	virtual const DrxTransform::CTransformPtr& GetTransform() const = 0;

	virtual const CClassProperties&      GetProperties() const = 0;
};

} // Schematyc
