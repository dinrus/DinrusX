// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>
#include <DinrusX/DinrusXSchema2/ScriptUserDocumentation.h>

namespace Schematyc2
{
	//////////////////////////////////////////////////////////////////////////
	void SScriptUserDocumentation::SetCurrentUserAsAuthor()
	{
		author = gEnv->pSystem->GetUserName();
	}

	//////////////////////////////////////////////////////////////////////////
	void SScriptUserDocumentation::Serialize(Serialization::IArchive& archive)
	{
		archive(author, "author", "Author(s)");
		archive(description, "description", "Description");
	}
}
