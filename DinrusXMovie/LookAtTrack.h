// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __LOOKATTRACK_H__
#define __LOOKATTRACK_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXMovie/IMovieSystem.h>
#include <DinrusX/DinrusXMovie/AnimTrack.h>

/** Look at target track, keys represent new lookat targets for entity.
 */
class CLookAtTrack : public TAnimTrack<SLookAtKey>
{
public:
	CLookAtTrack() : m_iAnimationLayer(-1) {}

	virtual bool           Serialize(XmlNodeRef& xmlNode, bool bLoading, bool bLoadEmptyTracks) override;

	virtual void           SerializeKey(SLookAtKey& key, XmlNodeRef& keyNode, bool bLoading) override;

	virtual CAnimParamType GetParameterType() const override          { return eAnimParamType_LookAt; }

	virtual int            GetAnimationLayerIndex() const override    { return m_iAnimationLayer; }
	virtual void           SetAnimationLayerIndex(int index) override { m_iAnimationLayer = index; }

private:
	int m_iAnimationLayer;
};

#endif // __LOOKATTRACK_H__
