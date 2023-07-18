// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		<DinrusX/CoreX/Platform/IPlatformOS.h> //Used to pass messages to the PlatformOS

		<DinrusX/np.h>
		<DinrusX/CoreX/Platform/OrbisSpecific.h>

		#include "DrxPSN2Response.h"

bool SDrxPSNBinAttribute::Clone(CDrxLobby* pLobby, SceNpMatching2RoomMemberBinAttrInternal& memberBinAttrInternal)
{
	m_id = memberBinAttrInternal.data.id;
	if (memberBinAttrInternal.data.ptr)
	{
		memcpy(m_data, memberBinAttrInternal.data.ptr, memberBinAttrInternal.data.size);
		m_dataSize = memberBinAttrInternal.data.size;
		return true;
	}
	else if (memberBinAttrInternal.data.size == 0)
	{
		m_dataSize = 0;
		return true;
	}
	else
	{
		m_dataSize = 0;
		return false;
	}
}

bool SDrxPSNBinAttribute::Clone(CDrxLobby* pLobby, SceNpMatching2RoomBinAttrInternal& roomBinAttrInternal)
{
	m_id = roomBinAttrInternal.data.id;
	if (roomBinAttrInternal.data.ptr)
	{
		memcpy(m_data, roomBinAttrInternal.data.ptr, roomBinAttrInternal.data.size);
		m_dataSize = roomBinAttrInternal.data.size;
		return true;
	}
	else if (roomBinAttrInternal.data.size == 0)
	{
		m_dataSize = 0;
		return true;
	}
	else
	{
		m_dataSize = 0;
		return false;
	}
}

bool SDrxPSNBinAttribute::Clone(CDrxLobby* pLobby, SceNpMatching2BinAttr& binAttr)
{
	m_id = binAttr.id;
	if (binAttr.ptr)
	{
		memcpy(m_data, binAttr.ptr, binAttr.size);
		m_dataSize = binAttr.size;
		return true;
	}
	else if (binAttr.size == 0)
	{
		m_dataSize = 0;
		return true;
	}
	else
	{
		m_dataSize = 0;
		return false;
	}
}

bool SDrxPSNUIntAttribute::Clone(CDrxLobby* pLobby, SceNpMatching2IntAttr& intAttr)
{
	m_id = intAttr.id;
	m_num = intAttr.num;
	return true;
}

bool SDrxPSNWorld::Clone(CDrxLobby* pLobby, SceNpMatching2World& world)
{
	m_worldId = world.worldId;
	m_numRooms = world.curNumOfRoom;
	m_numTotalRoomMembers = world.curNumOfTotalRoomMember;
	return true;
}

bool SDrxPSNRoomMemberDataInternal::Clone(CDrxLobby* pLobby, SceNpMatching2RoomMemberDataInternalA& member)
{
	memcpy((void*)&m_npId, &member.npId, sizeof(SceNpId));

	m_memberId = member.memberId;
	m_flagAttr = member.flagAttr;
	m_natType = member.natType;

	m_numBinAttributes = member.roomMemberBinAttrInternalNum;
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		if (!m_binAttributes[i].Clone(pLobby, member.roomMemberBinAttrInternal[i]))
		{
			return false;
		}
	}

	return true;
}

void SDrxPSNRoomMemberDataInternal::Release(CDrxLobby* pLobby)
{
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		m_binAttributes[i].Release(pLobby);
	}
}

bool SDrxPSNRoomDataExternal::Clone(CDrxLobby* pLobby, SceNpMatching2RoomDataExternalA& roomDataExternal)
{
	m_serverId = roomDataExternal.serverId;
	m_worldId = roomDataExternal.worldId;
	m_roomId = roomDataExternal.roomId;

	m_flagAttr = roomDataExternal.flagAttr;

	memcpy(&m_owner, roomDataExternal.owner, sizeof(SceNpId));

	m_numBinAttributes = roomDataExternal.roomBinAttrExternalNum;
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		if (!m_binAttributes[i].Clone(pLobby, roomDataExternal.roomBinAttrExternal[i]))
		{
			return false;
		}
	}
	m_numSearchableBinAttributes = roomDataExternal.roomSearchableBinAttrExternalNum;
	for (uint16 i = 0; i < m_numSearchableBinAttributes; i++)
	{
		if (!m_searchableBinAttributes[i].Clone(pLobby, roomDataExternal.roomSearchableBinAttrExternal[i]))
		{
			return false;
		}
	}
	m_numSearchableUIntAttributes = roomDataExternal.roomSearchableIntAttrExternalNum;
	for (uint16 i = 0; i < m_numSearchableUIntAttributes; i++)
	{
		if (!m_searchableUIntAttributes[i].Clone(pLobby, roomDataExternal.roomSearchableIntAttrExternal[i]))
		{
			return false;
		}
	}

	m_maxSlots = roomDataExternal.maxSlot;
	m_numPublicSlots = roomDataExternal.publicSlotNum;
	m_numPrivateSlots = roomDataExternal.privateSlotNum;
	m_numOpenPublicSlots = roomDataExternal.openPublicSlotNum;
	m_numOpenPrivateSlots = roomDataExternal.openPrivateSlotNum;
	m_numRoomMembers = roomDataExternal.curMemberNum;

	return true;
}

void SDrxPSNRoomDataExternal::Release(CDrxLobby* pLobby)
{
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		m_binAttributes[i].Release(pLobby);
	}
	for (uint16 i = 0; i < m_numSearchableBinAttributes; i++)
	{
		m_searchableBinAttributes[i].Release(pLobby);
	}
	for (uint16 i = 0; i < m_numSearchableUIntAttributes; i++)
	{
		m_searchableUIntAttributes[i].Release(pLobby);
	}
}

bool SDrxPSNRoomDataInternal::Clone(CDrxLobby* pLobby, SceNpMatching2RoomDataInternal& roomDataInternal)
{
	m_serverId = roomDataInternal.serverId;
	m_worldId = roomDataInternal.worldId;
	m_roomId = roomDataInternal.roomId;

	m_flagAttr = roomDataInternal.flagAttr;

	m_numBinAttributes = roomDataInternal.roomBinAttrInternalNum;
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		if (!m_binAttributes[i].Clone(pLobby, roomDataInternal.roomBinAttrInternal[i]))
		{
			return false;
		}
	}

	m_maxSlots = roomDataInternal.maxSlot;
		#if DRX_PLATFORM_ORBIS
	m_numPublicSlots = roomDataInternal.publicSlotNum;
	m_numPrivateSlots = roomDataInternal.privateSlotNum;
	m_numOpenPublicSlots = roomDataInternal.openPublicSlotNum;
	m_numOpenPrivateSlots = roomDataInternal.openPrivateSlotNum;
		#endif

	return true;
}

void SDrxPSNRoomDataInternal::Release(CDrxLobby* pLobby)
{
	for (uint16 i = 0; i < m_numBinAttributes; i++)
	{
		m_binAttributes[i].Release(pLobby);
	}
}

bool SDrxPSNRoomMemberDataInternalList::Clone(CDrxLobby* pLobby, SceNpMatching2RoomMemberDataInternalListA& memberList)
{
	m_numRoomMembers = memberList.membersNum;
	m_memHdl = TMemInvalidHdl;
	m_pRoomMembers = NULL;
	m_me = 0;
	m_owner = 0;

	uint32 nAllocSize = m_numRoomMembers * sizeof(SDrxPSNRoomMemberDataInternal);
	if (nAllocSize)
	{
		m_memHdl = pLobby->MemAlloc(nAllocSize);
		if (m_memHdl != TMemInvalidHdl)
		{
			m_pRoomMembers = (SDrxPSNRoomMemberDataInternal*)pLobby->MemGetPtr(m_memHdl);
			memset(m_pRoomMembers, 0, nAllocSize);

			SceNpMatching2RoomMemberDataInternalA* pMember = memberList.members;

			for (uint16 i = 0; i < m_numRoomMembers; i++)
			{
				if (!m_pRoomMembers[i].Clone(pLobby, *pMember))
				{
					return false;
				}

				if (pMember == memberList.me)
				{
					m_me = i;
				}
				if (pMember == memberList.owner)
				{
					m_owner = i;
				}

				pMember = pMember->next;
			}

			return true;
		}
	}

	return false;
}

void SDrxPSNRoomMemberDataInternalList::Release(CDrxLobby* pLobby)
{
	if (m_memHdl != TMemInvalidHdl)
	{
		for (uint16 i = 0; i < m_numRoomMembers; i++)
		{
			m_pRoomMembers[i].Release(pLobby);
		}
		pLobby->MemFree(m_memHdl);
	}
}

bool SDrxPSNGetWorldInfoListResponse::Clone(CDrxLobby* pLobby, SceNpMatching2GetWorldInfoListResponse* pResponse)
{
	m_numWorlds = pResponse->worldNum;
	m_pWorlds = NULL;

	uint32 nAllocSize = m_numWorlds * sizeof(SDrxPSNWorld);
	if (nAllocSize)
	{
		m_memHdl = pLobby->MemAlloc(nAllocSize);
		if (m_memHdl != TMemInvalidHdl)
		{
			m_pWorlds = (SDrxPSNWorld*)pLobby->MemGetPtr(m_memHdl);
			memset(m_pWorlds, 0, nAllocSize);

			for (uint16 i = 0; i < m_numWorlds; i++)
			{
				if (!m_pWorlds[i].Clone(pLobby, pResponse->world[i]))
				{
					return false;
				}
			}

			return true;
		}
	}

	return false;
}

void SDrxPSNGetWorldInfoListResponse::Release(CDrxLobby* pLobby)
{
	if (m_memHdl != TMemInvalidHdl)
	{
		for (uint16 i = 0; i < m_numWorlds; i++)
		{
			m_pWorlds[i].Release(pLobby);
		}
		pLobby->MemFree(m_memHdl);
	}
}

bool SDrxPSNCreateJoinRoomResponse::Clone(CDrxLobby* pLobby, SceNpMatching2CreateJoinRoomResponseA* pResponse)
{
	if (m_roomInfo.Clone(pLobby, *(pResponse->roomDataInternal)))
	{
		#if DRX_PLATFORM_ORBIS
		if (m_roomMembers.Clone(pLobby, pResponse->memberList))
		#endif
		{
			return true;
		}
	}
	return false;
}

void SDrxPSNCreateJoinRoomResponse::Release(CDrxLobby* pLobby)
{
	m_roomInfo.Release(pLobby);
	m_roomMembers.Release(pLobby);
}

bool SDrxPSNJoinRoomResponse::Clone(CDrxLobby* pLobby, SceNpMatching2JoinRoomResponseA* pResponse)
{
	if (m_roomInfo.Clone(pLobby, *(pResponse->roomDataInternal)))
	{
		#if DRX_PLATFORM_ORBIS
		if (m_roomMembers.Clone(pLobby, pResponse->memberList))
		#endif
		{
			return true;
		}
	}
	return false;
}

void SDrxPSNJoinRoomResponse::Release(CDrxLobby* pLobby)
{
	m_roomInfo.Release(pLobby);
	m_roomMembers.Release(pLobby);
}

bool SDrxPSNSearchRoomResponse::Clone(CDrxLobby* pLobby, SceNpMatching2SearchRoomResponseA* pResponse)
{
	m_numRooms = 0;
	m_memHdl = TMemInvalidHdl;
	m_pRooms = NULL;

	SceNpMatching2RoomDataExternalA* pRoom = pResponse->roomDataExternal;
	while (pRoom)
	{
		m_numRooms++;
		pRoom = pRoom->next;
	}

	uint32 nAllocSize = m_numRooms * sizeof(SDrxPSNRoomDataExternal);
	if (nAllocSize == 0)
	{
		return true;
	}
	else
	{
		m_memHdl = pLobby->MemAlloc(nAllocSize);
		if (m_memHdl != TMemInvalidHdl)
		{
			m_pRooms = (SDrxPSNRoomDataExternal*)pLobby->MemGetPtr(m_memHdl);
			memset(m_pRooms, 0, nAllocSize);

			pRoom = pResponse->roomDataExternal;

			for (uint16 i = 0; i < m_numRooms; i++)
			{
				if (!m_pRooms[i].Clone(pLobby, *pRoom))
				{
					return false;
				}
				pRoom = pRoom->next;
			}

			return true;
		}
	}

	return false;
}

bool SDrxPSNSearchRoomResponse::Clone(CDrxLobby* pLobby, SceNpMatching2GetRoomDataExternalListResponseA* pResponse)
{
	m_numRooms = pResponse->roomDataExternalNum;
	m_memHdl = TMemInvalidHdl;
	m_pRooms = NULL;

	uint32 nAllocSize = m_numRooms * sizeof(SDrxPSNRoomDataExternal);
	if (nAllocSize == 0)
	{
		return true;
	}
	else
	{
		m_memHdl = pLobby->MemAlloc(nAllocSize);
		if (m_memHdl != TMemInvalidHdl)
		{
			m_pRooms = (SDrxPSNRoomDataExternal*)pLobby->MemGetPtr(m_memHdl);
			memset(m_pRooms, 0, nAllocSize);

			SceNpMatching2RoomDataExternalA* pRoom = pResponse->roomDataExternal;

			for (uint16 i = 0; i < m_numRooms; i++)
			{
				if (!m_pRooms[i].Clone(pLobby, *pRoom))
				{
					return false;
				}
				pRoom = pRoom->next;
			}

			return true;
		}
	}

	return false;
}

void SDrxPSNSearchRoomResponse::Release(CDrxLobby* pLobby)
{
	if (m_memHdl != TMemInvalidHdl)
	{
		for (uint16 i = 0; i < m_numRooms; i++)
		{
			m_pRooms[i].Release(pLobby);
		}
		pLobby->MemFree(m_memHdl);
	}
}

bool SDrxPSNSignalingGetPingInfoResponse::Clone(CDrxLobby* pLobby, SceNpMatching2SignalingGetPingInfoResponse* pResponse)
{
	m_serverId = pResponse->serverId;
	m_worldId = pResponse->worldId;
	m_roomId = pResponse->roomId;
	m_rtt = pResponse->rtt;

	return true;
}

bool SDrxPSNRoomMemberUpdateInfoResponse::Clone(CDrxLobby* pLobby, SceNpMatching2RoomMemberUpdateInfoA* pResponse)
{
	SceNpMatching2RoomMemberDataInternalA* pMember = (SceNpMatching2RoomMemberDataInternalA*)pResponse->roomMemberDataInternal;

	if (!m_member.Clone(pLobby, *pMember))
	{
		return false;
	}

	m_eventCause = pResponse->eventCause;

	return true;
}

bool SDrxPSNRoomMemberUpdateInfoResponse::Clone(CDrxLobby* pLobby, SceNpMatching2RoomMemberDataInternalUpdateInfoA* pResponse)
{
	SceNpMatching2RoomMemberDataInternalA* pMember = (SceNpMatching2RoomMemberDataInternalA*)pResponse->newRoomMemberDataInternal;

	if (!m_member.Clone(pLobby, *pMember))
	{
		return false;
	}

	m_eventCause = PSN_OK;

	return true;
}

void SDrxPSNRoomMemberUpdateInfoResponse::Release(CDrxLobby* pLobby)
{
	m_member.Release(pLobby);
}

bool SDrxPSNRoomUpdateInfoResponse::Clone(CDrxLobby* pLobby, SceNpMatching2RoomUpdateInfo* pResponse)
{
	m_eventCause = pResponse->eventCause;
	m_errorCode = pResponse->errorCode;

	return true;
}

TMemHdl CDrxPSNSupport::CloneResponse(SceNpMatching2Event eventType, const void* pData, uint32* pReturnMemSize)
{
	*pReturnMemSize = 0;
	TMemHdl memHdl = TMemInvalidHdl;

	CDrxLobby* pLobby = GetLobby();
	if (pLobby)
	{
		// We only need to clone a limited set of eventTypes
		switch (eventType)
		{
		case REQUEST_EVENT_GET_WORLD_INFO_LIST:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNGetWorldInfoListResponse));
				if (memHdl)
				{
					SDrxPSNGetWorldInfoListResponse* pResponse = (SDrxPSNGetWorldInfoListResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2GetWorldInfoListResponse*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNGetWorldInfoListResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case REQUEST_EVENT_SEARCH_ROOM:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNSearchRoomResponse));
				if (memHdl)
				{
					SDrxPSNSearchRoomResponse* pResponse = (SDrxPSNSearchRoomResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2SearchRoomResponseA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNSearchRoomResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case REQUEST_EVENT_JOIN_ROOM:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNJoinRoomResponse));
				if (memHdl)
				{
					SDrxPSNJoinRoomResponse* pResponse = (SDrxPSNJoinRoomResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2JoinRoomResponseA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNJoinRoomResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case REQUEST_EVENT_CREATE_JOIN_ROOM:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNCreateJoinRoomResponse));
				if (memHdl)
				{
					SDrxPSNCreateJoinRoomResponse* pResponse = (SDrxPSNCreateJoinRoomResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2CreateJoinRoomResponseA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNCreateJoinRoomResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case REQUEST_EVENT_LEAVE_ROOM:
			{
		#if DRX_PLATFORM_ORBIS
				// currently no need to make a LeaveRoom response clone on Orbis - we don't use it.
				// memHdl = pLobby->MemAlloc(sizeof(SDrxPSNLeaveRoomResponse));
				// if (memHdl)
				// {
				//   SDrxPSNLeaveRoomResponse* pResponse = (SDrxPSNLeaveRoomResponse*)pLobby->MemGetPtr(memHdl);
				//	 if (pResponse->Clone(pLobby, (SceNpMatching2LeaveRoomResponse*)pData))
				//	 {
				//		 *pReturnMemSize = sizeof(SDrxPSNLeaveRoomResponse);
				//	 }
				//	 else
				//	 {
				//		 pResponse->Release(pLobby);
				//		 memHdl = TMemInvalidHdl;
				//	 }
				// }
		#endif
			}
			break;
		case REQUEST_EVENT_GET_ROOM_DATA_EXTERNAL_LIST:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNSearchRoomResponse));
				if (memHdl)
				{
					SDrxPSNSearchRoomResponse* pResponse = (SDrxPSNSearchRoomResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2GetRoomDataExternalListResponseA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNSearchRoomResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case REQUEST_EVENT_SIGNALING_GET_PING_INFO:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNSignalingGetPingInfoResponse));
				if (memHdl)
				{
					SDrxPSNSignalingGetPingInfoResponse* pResponse = (SDrxPSNSignalingGetPingInfoResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2SignalingGetPingInfoResponse*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNSignalingGetPingInfoResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;

		case ROOM_EVENT_MEMBER_JOINED:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNRoomMemberUpdateInfoResponse));
				if (memHdl)
				{
					SDrxPSNRoomMemberUpdateInfoResponse* pResponse = (SDrxPSNRoomMemberUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2RoomMemberUpdateInfoA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNRoomMemberUpdateInfoResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case ROOM_EVENT_MEMBER_LEFT:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNRoomMemberUpdateInfoResponse));
				if (memHdl)
				{
					SDrxPSNRoomMemberUpdateInfoResponse* pResponse = (SDrxPSNRoomMemberUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2RoomMemberUpdateInfoA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNRoomMemberUpdateInfoResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case ROOM_EVENT_KICKED_OUT:
		case ROOM_EVENT_ROOM_DESTROYED:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNRoomUpdateInfoResponse));
				if (memHdl)
				{
					SDrxPSNRoomUpdateInfoResponse* pResponse = (SDrxPSNRoomUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2RoomUpdateInfo*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNRoomUpdateInfoResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		case ROOM_EVENT_UPDATED_ROOM_MEMBER_DATA_INTERNAL:
			{
				memHdl = pLobby->MemAlloc(sizeof(SDrxPSNRoomMemberUpdateInfoResponse));
				if (memHdl)
				{
					SDrxPSNRoomMemberUpdateInfoResponse* pResponse = (SDrxPSNRoomMemberUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					if (pResponse->Clone(pLobby, (SceNpMatching2RoomMemberDataInternalUpdateInfoA*)pData))
					{
						*pReturnMemSize = sizeof(SDrxPSNRoomMemberUpdateInfoResponse);
					}
					else
					{
						pResponse->Release(pLobby);
						memHdl = TMemInvalidHdl;
					}
				}
			}
			break;
		default:
			break;
		}
	}

	return memHdl;
}

void CDrxPSNSupport::ReleaseClonedResponse(SceNpMatching2Event eventType, TMemHdl memHdl)
{
	if (memHdl != TMemInvalidHdl)
	{
		CDrxLobby* pLobby = GetLobby();
		if (pLobby)
		{
			switch (eventType)
			{
			case REQUEST_EVENT_GET_WORLD_INFO_LIST:
				{
					SDrxPSNGetWorldInfoListResponse* pResponse = (SDrxPSNGetWorldInfoListResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case REQUEST_EVENT_SEARCH_ROOM:
				{
					SDrxPSNSearchRoomResponse* pResponse = (SDrxPSNSearchRoomResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case REQUEST_EVENT_JOIN_ROOM:
				{
					SDrxPSNJoinRoomResponse* pResponse = (SDrxPSNJoinRoomResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case REQUEST_EVENT_CREATE_JOIN_ROOM:
				{
					SDrxPSNCreateJoinRoomResponse* pResponse = (SDrxPSNCreateJoinRoomResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case REQUEST_EVENT_LEAVE_ROOM:
				{
		#if DRX_PLATFORM_ORBIS
					// currently no need to make a LeaveRoom response clone on Orbis - we don't use it
					// SDrxPSNLeaveRoomResponse* pResponse = (SDrxPSNLeaveRoomResponse*)pLobby->MemGetPtr(memHdl);
					// pResponse->Release(pLobby);
		#endif
				}
				break;
			case REQUEST_EVENT_GET_ROOM_DATA_EXTERNAL_LIST:
				{
					SDrxPSNSearchRoomResponse* pResponse = (SDrxPSNSearchRoomResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case REQUEST_EVENT_SIGNALING_GET_PING_INFO:
				{
					SDrxPSNSignalingGetPingInfoResponse* pResponse = (SDrxPSNSignalingGetPingInfoResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;

			case ROOM_EVENT_MEMBER_JOINED:
				{
					SDrxPSNJoinRoomResponse* pResponse = (SDrxPSNJoinRoomResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case ROOM_EVENT_MEMBER_LEFT:
				{
					SDrxPSNRoomMemberUpdateInfoResponse* pResponse = (SDrxPSNRoomMemberUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case ROOM_EVENT_KICKED_OUT:
			case ROOM_EVENT_ROOM_DESTROYED:
				{
					SDrxPSNRoomUpdateInfoResponse* pResponse = (SDrxPSNRoomUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			case ROOM_EVENT_UPDATED_ROOM_MEMBER_DATA_INTERNAL:
				{
					SDrxPSNRoomMemberUpdateInfoResponse* pResponse = (SDrxPSNRoomMemberUpdateInfoResponse*)pLobby->MemGetPtr(memHdl);
					pResponse->Release(pLobby);
				}
				break;
			default:
				break;
			}

			pLobby->MemFree(memHdl);
		}
	}
}

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS
