// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
StringUtils.cpp
*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/StringUtils.h>
#include <DinrusX/DinrusXGame/DrxWatch.h>
#include <DinrusX/DinrusXGame/UI/HUD/HUDUtils.h>

static bool s_stringUtils_assertEnabled = true;

//--------------------------------------------------------------------------------
size_t cry_copyStringUntilFindChar(char * destination, const char * source, size_t bufferLength, char until)
{
	size_t reply = 0;

	DRX_ASSERT(destination);
	DRX_ASSERT(source);

	if (bufferLength)
	{
		size_t i;
		for (i = 0; source[i] && source[i] != until && (i + 1) < bufferLength; ++ i)
		{
			destination[i] = source[i];
		}
		destination[i] = '\0';
		reply = (source[i] == until) ? (i + 1) : 0;
	}

	return reply;
}

//--------------------------------------------------------------------------------

#ifndef cry_displayMemInHexAndAscii
void cry_displayMemInHexAndAscii(const char * startEachLineWith, const void * data, int size, ITextOutputHandler & output, const int bytesPerLine)
{
	if (size > 0 && data)
	{
		const unsigned char * charData = (const unsigned char *) data;
		string hexLine, asciiLine;
		int bytesOnThisLineSoFar = 0;
		int padTo = 0;

		while (size)
		{
			if (++ bytesOnThisLineSoFar > bytesPerLine)
			{
				output.DoOutput(string().Format("%s%s  %s", startEachLineWith, hexLine.c_str(), asciiLine.c_str()));
				padTo = hexLine.length();
				bytesOnThisLineSoFar -= bytesPerLine;
				asciiLine = "";
				hexLine = "";
			}

			hexLine = hexLine + string().Format("%02x", (int) * charData);

			if ((bytesOnThisLineSoFar & 7) == 0 && (bytesOnThisLineSoFar != bytesPerLine))
			{
				hexLine += " ";
			}

			asciiLine = asciiLine + string().Format("%c", (*charData >= 32 && *charData != 127) ? *charData : '.');

			++ charData;
			-- size;
		}

		output.DoOutput(string().Format("%s%s%s  %s", startEachLineWith, hexLine.c_str(), padTo ? string(padTo - hexLine.length(), ' ').c_str() : "", asciiLine.c_str()));
	}
	else
	{
		output.DoOutput(string().Format("%sPTR=%p SIZE=%d", startEachLineWith, size));
	}
}

//---------------------------------------------------------------------
void CDrxWatchOutputHandler::DoOutput(const char * text)
{
	DrxWatch ("%s", text);
}

//---------------------------------------------------------------------
void CDrxLogOutputHandler::DoOutput(const char * text)
{
	DrxLog ("%s", text);
}

//---------------------------------------------------------------------
void CDrxLogAlwaysOutputHandler::DoOutput(const char * text)
{
	DrxLogAlways ("%s", text);
}
#endif

//---------------------------------------------------------------------
const char * GetTimeString( int secs, bool useShortForm/*=false*/, bool includeSeconds/*=true*/, bool useSingleLetters/*=false*/ )
{
	int d, h, m, s;

	CGame::ExpandTimeSeconds(secs, d, h, m, s);

	static DrxFixedStringT<64> result;
	result.clear();

	if (useShortForm)
	{
		if(includeSeconds)
		{
			if (d>0)
				result.Format("%.2d:%.2d:%.2d:%.2d", d, h, m, s);
			else if(h>0)
				result.Format("%.2d:%.2d:%.2d", h, m, s);
			else
				result.Format("%.2d:%.2d", m, s);
		}
		else
		{
			if (d>0)
				result.Format("%.2d:%.2d:%.2d", d, h, m);
			else
				result.Format("%.2d:%.2d", h, m);
		}
	}
	else
	{
    if ( useSingleLetters == false )
    {
		  if (d==1)
			  result.Format("%d %s",d, CHUDUtils::LocalizeString("@ui_day"));
		  else if (d>1)
			  result.Format("%d %s",d, CHUDUtils::LocalizeString("@ui_days"));

		  if (h==1)
			  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", h, CHUDUtils::LocalizeString("@ui_hr"));
		  else if (h>1 || d>0)
			  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", h, CHUDUtils::LocalizeString("@ui_hrs"));

		  if (m==1)
			  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", m, CHUDUtils::LocalizeString("@ui_min"));
		  else if (m>1 || h>0 || d>0)
			  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", m, CHUDUtils::LocalizeString("@ui_mins"));

		  if(includeSeconds)
		  {
			  if (s==1)
				  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", s, CHUDUtils::LocalizeString("@ui_sec"));
			  else
				  result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", s, CHUDUtils::LocalizeString("@ui_secs"));
		  }
    }
    else
    {
      if (d>0)
        result.Format("%d %s",d, CHUDUtils::LocalizeString("@ui_mp_days"));

      if (h>0 || d>0)
        result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", h, CHUDUtils::LocalizeString("@ui_mp_hrs"));

      if (m>0 || h>0 || d>0)
        result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", m, CHUDUtils::LocalizeString("@ui_mp_mins"));

      if(includeSeconds)
      {
        if (s>0 || (h==0 && d==0 && m==0))
          result.Format("%s%s%d %s",result.c_str(), result.empty()?"":" ", s, CHUDUtils::LocalizeString("@ui_mp_sec"));
      }
    }
	}

	return result.c_str();
}

//---------------------------------------------------------------------
const char * GetTimeString( float secs, bool useShortForm/*=false*/, bool includeSeconds/*=true*/, bool useSingleLetters/*=false*/ )
{
	return GetTimeString(int_round(secs), useShortForm, includeSeconds, useSingleLetters);
}
