// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Rename IEnvInterfaceFunction to IEnvFunctionSignature/IEnvFunctionPrototype and move to separate header?

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>
#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{

// Forward declare classes.
class CAnyConstPtr;

struct IEnvInterfaceFunction : public IEnvElementBase<EEnvElementType::InterfaceFunction>
{
	virtual ~IEnvInterfaceFunction() {}

	virtual uint32       GetInputCount() const = 0;
	virtual const char*  GetInputName(uint32 inputIdx) const = 0;
	virtual const char*  GetInputDescription(uint32 inputIdx) const = 0;
	virtual CAnyConstPtr GetInputValue(uint32 inputIdx) const = 0;
	virtual uint32       GetOutputCount() const = 0;
	virtual const char*  GetOutputName(uint32 outputIdx) const = 0;
	virtual const char*  GetOutputDescription(uint32 outputIdx) const = 0;
	virtual CAnyConstPtr GetOutputValue(uint32 outputIdx) const = 0;
};

struct IEnvInterface : public IEnvElementBase<EEnvElementType::Interface>
{
	virtual ~IEnvInterface() {}
};

} // Schematyc
