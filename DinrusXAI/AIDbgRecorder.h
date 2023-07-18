// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/********************************************************************
   -------------------------------------------------------------------------
   File name:   AIDbgRecorder.h
   Описание: Простой текстовый записыватель событий отладки ИИ.

   -------------------------------------------------------------------------
   History:
   - 1:07:2005 : Created by Kirill Bulatsev
   -19:11:2008 : Separated out by Matthew

   Notes:        Really, this class is two separate debuggers - consider splitting
              Move the access point to gAIEnv
              Only creates the files on files on first logging - add some kind of init

 *********************************************************************/

#ifndef __AIDBGRECORDER_H__
#define __AIDBGRECORDER_H__

#pragma once

#ifdef DRXAISYS_DEBUG

// Простой текстовый записыватель отладки.
// Полностью независим от CAIRecorder, который более замысловат.
class CAIDbgRecorder
{
public:

	CAIDbgRecorder() {};
	~CAIDbgRecorder() {};

	bool IsRecording(const IAIObject* pTarget, IAIRecordable::e_AIDbgEvent event) const;
	void Record(const IAIObject* pTarget, IAIRecordable::e_AIDbgEvent event, const char* pString) const;

protected:
	void InitFile() const;
	void InitFileSecondary() const;

	void LogString(const char* pString) const;
	void LogStringSecondary(const char* pString) const;

	// Empty indicates currently unused
	// Has to be mutable right now because it changes on first logging
	mutable string m_sFile, m_sFileSecondary;
};

#endif //DRXAISYS_DEBUG

#endif //__AIDBGRECORDER_H__
