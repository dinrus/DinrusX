// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IEnvRegistrar;

typedef std::function<void (IEnvRegistrar&)> EnvPackageCallback;

struct IEnvPackage
{
	virtual ~IEnvPackage() {}

	virtual DrxGUID              GetGUID() const = 0;
	virtual const char*        GetName() const = 0;
	virtual const char*        GetAuthor() const = 0;
	virtual const char*        GetDescription() const = 0;
	virtual EnvPackageCallback GetCallback() const = 0;
};

} // Schematyc
