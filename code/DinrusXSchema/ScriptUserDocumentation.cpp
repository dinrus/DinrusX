// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

#include <DinrusX/CoreX/Serialization/STL.h>

namespace Schematyc
{
void SScriptUserDocumentation::SetCurrentUserAsAuthor()
{
	author = gEnv->pSystem->GetUserName();
}

void SScriptUserDocumentation::Serialize(Serialization::IArchive& archive)
{
	archive(author, "author", "Author(s)");
	archive(description, "description", "Description");
}
}
