// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// #SchematycTODO : Resolve disconnect between factory method used to construct elements during serialization and in place methods used when editing?
// #SchematycTODO : Allocate elements in pools?
class CScriptElementFactory
{
public:

	IScriptElementPtr CreateElement(EScriptElementType elementType);
};
} // Schematyc
