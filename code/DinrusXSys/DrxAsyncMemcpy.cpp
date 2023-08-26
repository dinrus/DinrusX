// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/CoreX/Thread/IJobUpr_JobDelegator.h>

namespace
{
static void drxAsyncMemcpy_Int(
  void* dst
  , const void* src
  , size_t size
  , int nFlags
  , volatile int* sync)
{
	drxMemcpy(dst, src, size, nFlags);
	if (sync)
		DrxInterlockedDecrement(sync);
}
}

DECLARE_JOB("DrxAsyncMemcpy", TDrxAsyncMemcpy, drxAsyncMemcpy_Int);

#if !defined(DRX_ASYNC_MEMCPY_DELEGATE_TO_DRXSYSTEM)
DRX_ASYNC_MEMCPY_API void drxAsyncMemcpy(
#else
DRX_ASYNC_MEMCPY_API void _drxAsyncMemcpy(
#endif
  void* dst
  , const void* src
  , size_t size
  , int nFlags
  , volatile int* sync)
{
	TDrxAsyncMemcpy job(dst, src, size, nFlags, sync);
	job.Run();
}
