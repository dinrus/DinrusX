// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __booltrack_h__
#define __booltrack_h__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXMovie/IMovieSystem.h>
#include <DinrusX/DinrusXMovie/AnimTrack.h>

/** Boolean track, every key on this track negates boolean value.
 */
class CBoolTrack : public TAnimTrack<SBoolKey>
{
public:
	CBoolTrack(const CAnimParamType& paramType);

	virtual EAnimValue        GetValueType() override           { return eAnimValue_Bool; };
	virtual CAnimParamType    GetParameterType() const override { return m_paramType; }

	virtual TMovieSystemValue GetValue(SAnimTime time) const override;
	virtual TMovieSystemValue GetDefaultValue() const override;
	virtual void              SetDefaultValue(const TMovieSystemValue& defaultValue) override;

	virtual void              SerializeKey(SBoolKey& key, XmlNodeRef& keyNode, bool bLoading) override {};

private:
	bool           m_bDefaultValue;
	CAnimParamType m_paramType;
};

#endif // __booltrack_h__
