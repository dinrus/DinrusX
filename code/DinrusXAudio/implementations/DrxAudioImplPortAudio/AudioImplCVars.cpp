// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAudio/StdAfx.h>
#include <DinrusX/DinrusXAudio/AudioImplCVars.h>
#include <DinrusX/DinrusXSys/IConsole.h>

namespace DrxAudio
{
namespace Impl
{
namespace PortAudio
{
//////////////////////////////////////////////////////////////////////////
void CCVars::RegisterVariables()
{
}

//////////////////////////////////////////////////////////////////////////
void CCVars::UnregisterVariables()
{
	IConsole* const pConsole = gEnv->pConsole;

	if (pConsole != nullptr)
	{
	}
}
} // namespace PortAudio
} // namespace Impl
} // namespace DrxAudio
