// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

struct IScriptInterfaceFunction : public IScriptElementBase<EScriptElementType::InterfaceFunction>
{
	virtual ~IScriptInterfaceFunction() {}

	virtual const char*  GetAuthor() const = 0;
	virtual const char*  GetDescription() const = 0;
	virtual uint32       GetInputCount() const = 0;
	virtual const char*  GetInputName(uint32 inputIdx) const = 0;
	virtual CAnyConstPtr GetInputValue(uint32 inputIdx) const = 0;
	virtual uint32       GetOutputCount() const = 0;
	virtual const char*  GetOutputName(uint32 outputIdx) const = 0;
	virtual CAnyConstPtr GetOutputValue(uint32 outputIdx) const = 0;
};
} // Schematyc
