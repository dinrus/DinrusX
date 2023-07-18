// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Unify uri format used by CreateUri and Execute.

#pragma once

#include <DinrusX/DinrusXSys/IConsole.h>

#include <DinrusX/DinrusXSchema2/GUID.h>
#include <DinrusX/DinrusXSchema2/IString.h>
#include <DinrusX/DinrusXSchema2/StackString.h>

namespace Schematyc2
{
namespace DrxLinkUtils
{
enum class ECommand
{
	None = 0,     // Do nothing.
	Show          // Show element in editor.
};

inline bool CreateUri(IString& output, ECommand command, const Schematyc2::SGUID& elementGUID, const Schematyc2::SGUID& detailGUID = Schematyc2::SGUID())
{
	CStackString commandLine;
	switch (command)
	{
	case ECommand::Show:
		{
			CStackString temp;
			commandLine = "sc_rpcShowLegacy ";
			Schematyc2::StringUtils::SysGUIDToString(elementGUID.sysGUID, temp);
			commandLine.append(temp);
			commandLine.append(" ");
			Schematyc2::StringUtils::SysGUIDToString(detailGUID.sysGUID, temp);
			commandLine.append(temp);
			break;
		}
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

inline void ExecuteCommand(ECommand command, const Schematyc2::SGUID& elementGUID, const Schematyc2::SGUID& detailGUID = Schematyc2::SGUID())
{
	CStackString uri;
	CreateUri(uri, command, elementGUID, detailGUID);
	ExecuteUri(uri.c_str());
}
} // DrxLinkUtils
} // Schematyc
