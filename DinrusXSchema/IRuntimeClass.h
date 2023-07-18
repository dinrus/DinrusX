// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace Schematyc
{

// Forward declare interfaces.
struct IObjectProperties;
// Forward declare classes.
class CAnyConstPtr;
class CScratchpad;

struct IRuntimeClass
{
	virtual ~IRuntimeClass() {}

	virtual time_t                   GetTimeStamp() const = 0;
	virtual DrxGUID                    GetGUID() const = 0;
	virtual const char*              GetName() const = 0;

	virtual const IObjectProperties& GetDefaultProperties() const = 0;
	virtual DrxGUID                    GetEnvClassGUID() const = 0;
	virtual CAnyConstPtr             GetEnvClassProperties() const = 0;
	virtual const CScratchpad&       GetScratchpad() const = 0;
};

} // Schematyc
