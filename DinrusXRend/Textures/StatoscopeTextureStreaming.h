// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef STATOSCOPETEXTURESTREAMING_H
#define STATOSCOPETEXTURESTREAMING_H

#include <DinrusX/DinrusXSys/IStatoscope.h>

#if ENABLE_STATOSCOPE

	#include <DinrusX/DinrusXRend/TextureStreamPool.h>

struct SStatoscopeTextureStreamingDG : public IStatoscopeDataGroup
{
	virtual void         Enable();
	virtual void         Disable();

	virtual SDescription GetDescription() const;
	virtual void         Write(IStatoscopeFrameRecord& fr);

private:
	float GetTextureRequests();
	float GetTextureRenders();
	float GetTexturePoolUsage();
	float GetTexturePoolWanted();
	float GetTextureUpdates();
};

struct SStatoscopeTextureStreamingItemsDG : public IStatoscopeDataGroup
{
	virtual void         Enable();
	virtual void         Disable();

	virtual SDescription GetDescription() const;
	virtual uint32       PrepareToWrite();
	virtual void         Write(IStatoscopeFrameRecord& fr);

private:
	std::vector<CTexture::WantedStat> m_statsTexWantedLists[2];
};

	#ifndef _RELEASE

struct SStatoscopeTextureStreamingPoolDG : public IStatoscopeDataGroup
{
	virtual void         Enable();
	virtual void         Disable();

	virtual SDescription GetDescription() const;
	virtual uint32       PrepareToWrite();
	virtual void         Write(IStatoscopeFrameRecord& fr);

private:
	std::vector<CTextureStreamPoolMgr::SPoolStats> m_ps;
};

	#endif

#endif

#endif
