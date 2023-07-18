// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Unify uri format used by CreateUri and Execute.

#pragma once

#include <DinrusX/DinrusXSys/IConsole.h>

#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/IString.h>
#include <DinrusX/DinrusXSchema/StackString.h>

namespace Schematyc
{
namespace DrxLinkUtils
{
enum class ECommand
{
	None = 0,     // Do nothing.
	Show          // Show element in editor.
};

inline bool CreateUri(IString& output, ECommand command, const DrxGUID& elementGUID, const DrxGUID& detailGUID = DrxGUID())
{
	CStackString commandLine;
	if(command == ECommand::Show)
		{
			CStackString temp;
			commandLine = "sc_rpcShow ";
			GUID::ToString(temp, elementGUID);
			commandLine.append(temp);
			commandLine.append(" ");
			GUID::ToString(temp, detailGUID);
			commandLine.append(temp);
		
		}
	
	if (!commandLine.empty())
	{
		DrxLinkService::CDrxLinkUriFactory drxLinkFactory("editor", DrxLinkService::ELinkType::Commands);
		drxLinkFactory.AddCommand(commandLine.c_str(), commandLine.length());
		output.assign(drxLinkFactory.GetUri());
		return true;
	}
	return false;
}

inline void ExecuteUri(const char* szUri)
{
	DrxLinkService::CDrxLink drxLink(szUri);
	const char* szCmd = drxLink.GetQuery("cmd1");
	if (szCmd && szCmd[0])
	{
		gEnv->pConsole->ExecuteString(szCmd);
	}
}

inline void ExecuteCommand(ECommand command, const DrxGUID& elementGUID, const DrxGUID& detailGUID = DrxGUID())
{
	CStackString uri;
	CreateUri(uri, command, elementGUID, detailGUID);
	ExecuteUri(uri.c_str());
}
} // DrxLinkUtils
} // Schematyc
