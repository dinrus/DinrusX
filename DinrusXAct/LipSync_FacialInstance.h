// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   LipSync_FacialInstance.h
//  Version:     v1.00
//  Created:     2014-08-29 by Christian Werle.
//  Описание: Automatic start of facial animation when a sound is being played back.
//               Legacy version that uses DinrusXAnimation's FacialInstance.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DinrusX/DinrusXAct/ILipSyncProvider.h>

class CLipSyncProvider_FacialInstance : public ILipSyncProvider
{
public:
	explicit CLipSyncProvider_FacialInstance(EntityId entityId);

	// ILipSyncProvider
	virtual void RequestLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	virtual void StartLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	virtual void PauseLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	virtual void UnpauseLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	virtual void StopLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	virtual void UpdateLipSync(IEntityAudioComponent* pProxy, const DrxAudio::ControlId audioTriggerId, const ELipSyncMethod lipSyncMethod) override;
	// ~ILipSyncProvider

	void FullSerialize(TSerialize ser);

private:
	void LipSyncWithSound(const DrxAudio::ControlId audioTriggerId, bool bStop = false);
	EntityId m_entityId;
};
DECLARE_SHARED_POINTERS(CLipSyncProvider_FacialInstance);

class CLipSync_FacialInstance : public CGameObjectExtensionHelper<CLipSync_FacialInstance, IGameObjectExtension>
{
public:
	// IGameObjectExtension
	virtual void                 Initialize() override {};
	virtual void                 GetMemoryUsage(IDrxSizer* pSizer) const override;
	virtual bool                 Init(IGameObject* pGameObject) override;
	virtual void                 PostInit(IGameObject* pGameObject) override;
	virtual void                 InitClient(int channelId) override;
	virtual void                 PostInitClient(int channelId) override;
	virtual bool                 ReloadExtension(IGameObject* pGameObject, const SEntitySpawnParams& params) override;
	virtual void                 PostReloadExtension(IGameObject* pGameObject, const SEntitySpawnParams& params) override;
	virtual void                 FullSerialize(TSerialize ser) override;
	virtual bool                 NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override;
	virtual void                 PostSerialize() override;
	virtual void                 SerializeSpawnInfo(TSerialize ser) override;
	virtual ISerializableInfoPtr GetSpawnInfo() override;
	virtual void                 Update(SEntityUpdateContext& ctx, int updateSlot) override;
	virtual void                 HandleEvent(const SGameObjectEvent& event) override;
	virtual void                 ProcessEvent(const SEntityEvent& event) override;
	virtual uint64               GetEventMask() const override;
	virtual void                 SetChannelId(uint16 id) override;
	virtual void                 PostUpdate(float frameTime) override;
	virtual void                 PostRemoteSpawn() override;
	// ~IGameObjectExtension

	// IEntityComponent
	virtual void OnShutDown() override;
	// ~IEntityComponent

private:
	void InjectLipSyncProvider();

private:
	CLipSyncProvider_FacialInstancePtr m_pLipSyncProvider;
};
