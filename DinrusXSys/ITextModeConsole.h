// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:
   Allows creation of text mode displays the for dedicated server
   -------------------------------------------------------------------------
   History:
   - Nov 22,2006:	Created by Craig Tiller

*************************************************************************/

#pragma once

struct ITextModeConsole
{
	// <interfuscator:shuffle>
	virtual ~ITextModeConsole() {}
	virtual Vec2_tpl<int> BeginDraw() = 0;
	virtual void          PutText(int x, int y, const char* msg) = 0;
	virtual void          EndDraw() = 0;
	virtual void          OnShutdown() = 0;

	virtual void          SetTitle(const char* title)    {}
	virtual void          SetHeader(const char* pHeader) {}
	// </interfuscator:shuffle>
};
