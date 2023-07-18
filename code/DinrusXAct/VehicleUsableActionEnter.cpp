// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Implements a usable action to enter a vehicle seat

   -------------------------------------------------------------------------
   History:
   - 19:01:2006: Created by Mathieu Pinard

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>
#include <DinrusX/DinrusXAct/GameObject.h>
#include <DinrusX/DinrusXAct/IActorSystem.h>
#include <DinrusX/DinrusXAct/IGameObject.h>
#include <DinrusX/DinrusXAct/IItem.h>
#include <DinrusX/DinrusXAct/IItemSystem.h>
#include <DinrusX/DinrusXAct/IVehicleSystem.h>
#include <DinrusX/DinrusXAct/Vehicle.h>
#include <DinrusX/DinrusXAct/VehicleSeat.h>
#include <DinrusX/DinrusXAct/VehicleSeatGroup.h>
#include <DinrusX/DinrusXAct/VehicleUsableActionEnter.h>

//------------------------------------------------------------------------
bool CVehicleUsableActionEnter::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	CVehicleParams enterTable = table.findChild("Enter");
	if (!enterTable)
		return false;

	m_pVehicle = static_cast<CVehicle*>(pVehicle);

	if (CVehicleParams seatsTables = enterTable.findChild("Seats"))
	{
		int c = seatsTables.getChildCount();
		int i = 0;
		m_seatIds.reserve(c);

		for (; i < c; i++)
		{
			CVehicleParams seatRef = seatsTables.getChild(i);

			if (const char* pSeatName = seatRef.getAttr("value"))
			{
				if (TVehicleSeatId seatId = m_pVehicle->GetSeatId(pSeatName))
					m_seatIds.push_back(seatId);
			}
		}
	}

	return !m_seatIds.empty();
}

//------------------------------------------------------------------------
int CVehicleUsableActionEnter::OnEvent(int eventType, SVehicleEventParams& eventParams)
{
	if (eventType == eVAE_IsUsable)
	{
		EntityId& userId = eventParams.entityId;

		for (TVehicleSeatIdVector::iterator ite = m_seatIds.begin(), end = m_seatIds.end(); ite != end; ++ite)
		{
			TVehicleSeatId seatId = *ite;
			IVehicleSeat* pSeat = m_pVehicle->GetSeatById(seatId);
			if (IsSeatAvailable(pSeat, userId))
			{
				eventParams.iParam = seatId;
				return 1;
			}
		}

		return 0;
	}
	else if (eventType == eVAE_OnUsed)
	{
		EntityId& userId = eventParams.entityId;

		IVehicleSeat* pSeat = m_pVehicle->GetSeatById(eventParams.iParam);

		if (IsSeatAvailable(pSeat, userId))
			return pSeat->Enter(userId);

		return -1;
	}

	return 0;
}

//------------------------------------------------------------------------
bool CVehicleUsableActionEnter::IsSeatAvailable(IVehicleSeat* pSeat, EntityId userId)
{
	if (!pSeat)
		return false;

	IActor* pActor = CDrxAction::GetDrxAction()->GetIActorSystem()->GetActor(userId);
	if (!static_cast<CVehicleSeat*>(pSeat)->IsFree(pActor))
		return false;

	return true;
}

void CVehicleUsableActionEnter::GetMemoryStatistics(IDrxSizer* s)
{
	s->Add(*this);
	s->AddContainer(m_seatIds);
}

DEFINE_VEHICLEOBJECT(CVehicleUsableActionEnter);
