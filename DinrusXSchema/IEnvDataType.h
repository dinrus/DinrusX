// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>

namespace Schematyc
{

// Forward declare classes.
class CCommonTypeDesc;

struct IEnvDataType : public IEnvElementBase<EEnvElementType::DataType>
{
	virtual ~IEnvDataType() {}

	virtual const CCommonTypeDesc& GetDesc() const = 0;
};

} // Schematyc
