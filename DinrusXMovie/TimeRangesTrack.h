// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __TIMERANGESTRACK_H__
#define __TIMERANGESTRACK_H__

#pragma once

//forward declarations.
#include <DinrusX/DinrusXMovie/IMovieSystem.h>
#include <DinrusX/DinrusXMovie/AnimTrack.h>

/** CTimeRangesTrack contains keys that represent generic time ranges
 */
class CTimeRangesTrack : public TAnimTrack<STimeRangeKey>
{
public:
	virtual void           SerializeKey(STimeRangeKey& key, XmlNodeRef& keyNode, bool bLoading) override;

	virtual CAnimParamType GetParameterType() const override { return eAnimParamType_TimeRanges; }

	int                    GetActiveKeyIndexForTime(const SAnimTime time);
};

#endif
