// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Executes an ASCII batch file of console commands...

   -------------------------------------------------------------------------
   History:
   - 19:04:2006   10:38 : Created by Jan Müller
   - 26:06:2006           Modified by Timur.

*************************************************************************/

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/ConsoleBatchFile.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/XConsole.h>
#include <DinrusX/CoreX/String/Path.h>
#include <stdio.h>
#include <DinrusX/DinrusXSys/System.h>
#include <DinrusX/DinrusXSys/SystemCFG.h>

IConsole* CConsoleBatchFile::m_pConsole = NULL;

void CConsoleBatchFile::Init()
{
	m_pConsole = gEnv->pConsole;
	REGISTER_COMMAND("exec", (ConsoleCommandFunc)ExecuteFileCmdFunc, 0, "executes a batch file of console commands");
}

//////////////////////////////////////////////////////////////////////////
void CConsoleBatchFile::ExecuteFileCmdFunc(IConsoleCmdArgs* args)
{
	if (!m_pConsole)
		Init();

	if (!args->GetArg(1))
		return;

	ExecuteConfigFile(args->GetArg(1));
}

//////////////////////////////////////////////////////////////////////////
bool CConsoleBatchFile::ExecuteConfigFile(const char* sFilename)
{
	if (!m_pConsole)
		Init();

	string filename = PathUtil::Make(gEnv->pSystem->GetRootFolder(), PathUtil::GetFile(sFilename));
	if (strlen(PathUtil::GetExt(filename)) == 0)
	{
		filename = PathUtil::ReplaceExtension(filename, "cfg");
	}

#if defined(CVARS_WHITELIST)
	bool ignoreWhitelist = true;
	if (stricmp(sFilename, "autoexec.cfg") == 0)
	{
		ignoreWhitelist = false;
	}
#endif // defined(CVARS_WHITELIST)

	//////////////////////////////////////////////////////////////////////////
	CDrxFile file;

	{
		const char* szLog = "Executing console batch file (try game,config,root):";
		string filenameLog;
		string sfn = PathUtil::GetFile(filename);

		if (!CSystemConfiguration::OpenFile(filename, file, IDrxPak::FOPEN_ONDISK))
		{
			DrxLog("%s \"%s\" not found!", szLog, filename.c_str());
			return false;
		}
		filenameLog = file.GetFilename();
		DrxLog("%s \"%s\" found in %s ...", szLog, filename.c_str(), filenameLog.c_str());
	}

	int nLen = file.GetLength();
	char* sAllText = new char[nLen + 16];
	file.ReadRaw(sAllText, nLen);
	sAllText[nLen] = '\0';
	sAllText[nLen + 1] = '\0';

	/*
	   This can't work properly as ShowConsole() can be called during the execution of the scripts,
	   which means bConsoleStatus is outdated and must not be set at the end of the function

	   bool bConsoleStatus = ((CXConsole*)m_pConsole)->GetStatus();
	   ((CXConsole*)m_pConsole)->SetStatus(false);
	 */

	char* strLast = sAllText + nLen;
	char* str = sAllText;
	while (str < strLast)
	{
		char* s = str;
		while (str < strLast && *str != '\n' && *str != '\r')
			str++;
		*str = '\0';
		str++;
		while (str < strLast && (*str == '\n' || *str == '\r'))
			str++;

		string strLine = s;

		//trim all whitespace characters at the beginning and the end of the current line and store its size
		strLine.Trim();
		size_t strLineSize = strLine.size();

		//skip comments, comments start with ";" or "--" but may have preceding whitespace characters
		if (strLineSize > 0)
		{
			if (strLine[0] == ';')
				continue;
			else if (strLine.find("--") == 0)
				continue;
		}
		//skip empty lines
		else
			continue;

#if defined(CVARS_WHITELIST)
		auto pWhiteList = gEnv->pSystem->GetCVarsWhiteList();
		if ((ignoreWhitelist) || (pWhiteList && pWhiteList->IsWhiteListed(strLine, false)))
#endif // defined(CVARS_WHITELIST)
		{
			m_pConsole->ExecuteString(strLine);
		}
#if defined(DEDICATED_SERVER)
	#if defined(CVARS_WHITELIST)
		else
		{
			gEnv->pSystem->GetILog()->LogError("Failed to execute command: '%s' as it is not whitelisted\n", strLine.c_str());
		}
	#endif // defined(CVARS_WHITELIST)
#endif   // defined(DEDICATED_SERVER)
	}
	// See above
	//	((CXConsole*)m_pConsole)->SetStatus(bConsoleStatus);

	delete[]sAllText;
	return true;
}
