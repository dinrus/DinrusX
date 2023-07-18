// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/DrxHash.h>

DrxHashStringId DrxHashStringId::GetIdForName( const char* _name )
{
	DRX_ASSERT(_name);

	return DrxHashStringId(_name);
}
