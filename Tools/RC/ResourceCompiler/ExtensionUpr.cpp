// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Разработка (C), Dinrus Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   extensionmanager.cpp
//  Version:     v1.00
//  Created:     5/11/2002 by Timur.
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <time.h>
#include "ExtensionUpr.h"
#include "IConverter.h"
#include "IResCompiler.h"       // IResourceCompiler
#include "IRCLog.h"             // IRCLog
#include "StringHelpers.h"

//////////////////////////////////////////////////////////////////////////
ExtensionUpr::ExtensionUpr()
{
}

//////////////////////////////////////////////////////////////////////////
ExtensionUpr::~ExtensionUpr()
{
	UnregisterAll();
}

//////////////////////////////////////////////////////////////////////////
IConverter* ExtensionUpr::FindConverter( const char *filename )  const
{
	string const strFilename = StringHelpers::MakeLowerCase(string(filename));

	for (size_t i = 0; i < m_extVector.size(); ++i)
	{
		if (StringHelpers::EndsWith(strFilename, m_extVector[i].first))
		{
			return m_extVector[i].second;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void ExtensionUpr::RegisterConverter(const char* name, IConverter* conv, IResourceCompiler* rc)
{
	assert(conv);
	assert(rc);

	m_converters.push_back(conv);

	string strExt;
	for (int i = 0; ; ++i)
	{
		const char* const ext = conv->GetExt(i);
		if (!ext || !ext[0])
		{
			break;
		}
		if (i)
		{
			strExt += ", ";
		}
		strExt += "\"";
		strExt += ext;
		strExt += "\"";

		const string extToAdd = StringHelpers::MakeLowerCase(string(".") + ext);
		m_extVector.push_back(ExtVector::value_type(extToAdd, conv));
	}

	if (strExt.empty())
	{
		RCLogError("    %s failed to provide list of extensions", name);
	}
	else
	{
		RCLog("    Registered %s (%s)", name, strExt.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
void ExtensionUpr::UnregisterAll()
{
	for (size_t i = 0; i < m_converters.size(); ++i)
	{
		IConverter *conv = m_converters[i];
		conv->Release();
	}
	m_converters.clear();

	m_extVector.clear();
}
