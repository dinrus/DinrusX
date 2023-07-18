// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __IPROGRESS_H__
#define __IPROGRESS_H__

class IProgress
{
public:
	virtual ~IProgress() {}

	virtual void StartProgress() = 0;
	virtual void ShowProgress(const char* pMessage, size_t progressValue, size_t maxProgressValue) = 0;
	virtual void FinishProgress() = 0;
};

#endif
