// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Object for ledge placement in the levels

-------------------------------------------------------------------------
History:
- 25:02:2012   Created by Benito Gangoso Rodriguez

*************************************************************************/
#pragma once

#ifndef _LEDGE_H_
#define _LEDGE_H_

#include <DinrusX/DinrusXAct/IGameObject.h>

class CLedgeObject : public CGameObjectExtensionHelper<CLedgeObject, IGameObjectExtension>
{
	struct LedgeProperties
	{
		LedgeProperties(const IEntity& entity);

		float ledgeCornerMaxAngle;
		float ledgeCornerEndAdjustAmount;
		bool ledgeFlipped;
		uint16 ledgeFlags_MainSide;
		uint16 ledgeFlags_OppositeSide;


	private:
		LedgeProperties();
	};

public:

	CLedgeObject();
	virtual ~CLedgeObject();

	// IGameObjectExtension
	virtual bool Init( IGameObject * pGameObject );
	virtual void InitClient( int channelId ) {};
	virtual void PostInit( IGameObject * pGameObject );
	virtual void PostInitClient( int channelId ) {};
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature );
	virtual void Release();
	virtual void FullSerialize( TSerialize ser ) {};
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return false; };
	virtual void PostSerialize() {};
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() {return 0;}
	virtual void Update( SEntityUpdateContext& ctx, int slot ) { };
	virtual void HandleEvent( const SGameObjectEvent& gameObjectEvent );
	virtual void ProcessEvent( SEntityEvent& entityEvent );
	virtual void SetChannelId( uint16 id ) {};
	virtual void SetAuthority( bool auth ) {};
	virtual void PostUpdate( float frameTime ) { DRX_ASSERT(false); }
	virtual void PostRemoteSpawn() {};
	virtual void GetMemoryUsage( IDrxSizer *pSizer ) const;
	// ~IGameObjectExtension

protected:

	virtual bool IsStatic() const
	{
		return false;
	}

private:

	void UpdateLocation();
	void ComputeLedgeMarkers();
	
	ILINE bool IsFlipped() const { return m_flipped; };

	bool m_flipped;
};

//////////////////////////////////////////////////////////////////////////

class CLedgeObjectStatic : public CLedgeObject
{
public:

	CLedgeObjectStatic();
	virtual ~CLedgeObjectStatic();

protected:

	virtual bool IsStatic() const
	{
		return true;
	}

};

#endif //_LEDGE_H_