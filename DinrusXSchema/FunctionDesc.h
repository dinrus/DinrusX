// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{

class CCommonFunctionDesc
{
public:

private:

	DrxGUID       m_guid;
	const char* m_szName = nullptr;
};

template<typename TYPE, TYPE FUNCTION_PTR> class CFunctionDesc : public CCommonFunctionDesc
{
};

} // Schematyc