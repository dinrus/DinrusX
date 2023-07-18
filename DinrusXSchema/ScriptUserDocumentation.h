// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

namespace Schematyc
{
struct SScriptUserDocumentation
{
	void SetCurrentUserAsAuthor();
	void Serialize(Serialization::IArchive& archive);

	string author;
	string description;
};
}
