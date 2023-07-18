// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/Delegate.h>

namespace Schematyc
{
enum class EValidatorMessageType
{
	Warning = 0,
	Error
};

typedef std::function<void(EValidatorMessageType, const char*)> Validator;
} // Schematyc
