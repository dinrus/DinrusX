// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Extension/IDrxUnknown.h>

struct SSystemInitParams;
struct SSystemGlobalEnvironment;

namespace Drx
{
	//! Base Interface for all engine module extensions.
	struct IDefaultModule : public IDrxUnknown
	{
		DRXINTERFACE_DECLARE_GUID(IDefaultModule, "f899cf66-1df0-4f61-a341-a8a7ffdf9de4"_drx_guid);

		// <interfuscator:shuffle>
		//! Retrieve name of the extension module.
		virtual const char* GetName() const = 0;

		//! Retrieve category for the extension module (DinrusX for standard modules).
		virtual const char* GetCategory() const = 0;

		//! This is called to initialize the new module.
		virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) = 0;
		// </interfuscator:shuffle>
	};
}