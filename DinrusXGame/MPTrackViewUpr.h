// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef MP_TRACK_VIEW_MANAGER
#define MP_TRACK_VIEW_MANAGER

#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/DinrusXMovie/IMovieSystem.h>

class CMPTrackViewUpr : public IMovieListener
{
public:
	CMPTrackViewUpr();
	~CMPTrackViewUpr();

	void Init();
	void Update();

	void Server_SynchAnimationTimes(CGameRules::STrackViewParameters& params); 
	void Client_SynchAnimationTimes(const CGameRules::STrackViewParameters& params);
	void AnimationRequested(const CGameRules::STrackViewRequestParameters& params);
	bool HasTrackviewFinished(const DrxHashStringId& id) const;

	IAnimSequence* FindTrackviewSequence(int trackviewId);

private:

	// IMovieListener
	virtual void OnMovieEvent(IMovieListener::EMovieEvent movieEvent, IAnimSequence* pAnimSequence);
	// ~IMovieListener

	int m_FinishedTrackViews[CGameRules::STrackViewParameters::sMaxTrackViews];
	float m_FinishedTrackViewTimes[CGameRules::STrackViewParameters::sMaxTrackViews];
	int m_FinishedTrackViewCount;
	bool m_movieListener;

};

#endif