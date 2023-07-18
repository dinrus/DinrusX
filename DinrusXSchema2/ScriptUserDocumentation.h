// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/IScriptFile.h>

namespace Schematyc2
{
	struct SScriptUserDocumentation
	{
		void SetCurrentUserAsAuthor();
		void Serialize(Serialization::IArchive& archive);

		string	author;
		string	description;
	};
}
