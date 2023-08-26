// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/WorkQueue.h>

void CWorkQueue::FlushEmpty()
{
	DoNothingOp nothing;
	m_jobQueue.Flush(nothing);
	Empty();
}
