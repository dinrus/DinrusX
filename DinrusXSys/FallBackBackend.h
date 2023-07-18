// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   FallBackBackEnd.h
//  Version:     v1.00
//  Created:     07/05/2011 by Christopher Bolte
//  Compilers:   Visual Studio.NET
// -------------------------------------------------------------------------
//  History:
////////////////////////////////////////////////////////////////////////////

#ifndef FallBackBackEnd_H_
#define FallBackBackEnd_H_

#include <DinrusX/CoreX/Thread/IJobUpr.h>

namespace JobUpr {
namespace FallBackBackEnd {

class CFallBackBackEnd : public IBackend
{
public:
	CFallBackBackEnd();
	~CFallBackBackEnd();

	bool   Init(uint32 nSysMaxWorker) { return true; }
	bool   ShutDown()                 { return true; }
	void   Update()                   {}

	void   AddJob(JobUpr::CJobDelegator& crJob, const JobUpr::TJobHandle cJobHandle, JobUpr::SInfoBlock& rInfoBlock);

	uint32 GetNumWorkerThreads() const { return 0; }

#if defined(JOBMANAGER_SUPPORT_FRAMEPROFILER)
	virtual IWorkerBackEndProfiler* GetBackEndWorkerProfiler() const { return 0; }
#endif
};

} // namespace FallBackBackEnd
} // namespace JobUpr

#endif // FallBackBackEnd_H_
