// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXSHAREDLOBBYPACKET_H__
#define __DRXSHAREDLOBBYPACKET_H__

#pragma once

<DinrusX/CoreX/Lobby/DrxLobbyPacket.h>
#include "DrxMatchMaking.h"

const uint32 DrxLobbyPacketErrorSize = DrxLobbyPacketUINT16Size;
const uint32 DrxLobbyPacketLobbySessionHandleSize = DrxLobbyPacketUINT8Size;

class CDrxSharedLobbyPacket : public CDrxLobbyPacket
{
public:
	void WritePacketHeader()
	{
		CDrxLobby* pLobby = (CDrxLobby*)CDrxLobby::GetLobby();

		WriteUINT8(pLobby->GetLobbyPacketID());

		if (pLobby->GetLobbyServiceType() == eCLS_Online)
		{
			// Online use WriteData since if there is no cross platform WriteData or WriteUINT64 can both be used but WriteData is faster.
			// Also if we want to support cross platform with games for windows live and xbox live WriteData must be used.
			WriteData(&m_packetHeader.fromUID.m_sid, DrxLobbyPacketUINT64Size);
		}
		else
		{
			// LAN always needs to use WriteUINT64 to keep cross platform compatibility.
			WriteUINT64(m_packetHeader.fromUID.m_sid);
		}

		WriteUINT16(m_packetHeader.fromUID.m_uid);
		WriteBool(m_packetHeader.reliable);

		if (m_packetHeader.reliable)
		{
			WriteUINT8(m_packetHeader.counterOut);
			WriteUINT8(m_packetHeader.counterIn);
		}
	}

	void ReadPacketHeader()
	{
		ReadUINT8();

		if (CDrxLobby::GetLobby()->GetLobbyServiceType() == eCLS_Online)
		{
			// Online use ReadData since if there is no cross platform ReadData or ReadUINT64 can both be used but ReadData is faster.
			// Also if we want to support cross platform with games for windows live and xbox live ReadData must be used.
			ReadData(&m_packetHeader.fromUID.m_sid, DrxLobbyPacketUINT64Size);
		}
		else
		{
			// LAN always needs to use ReadUINT64 to keep cross platform compatibility.
			m_packetHeader.fromUID.m_sid = ReadUINT64();
		}

		m_packetHeader.fromUID.m_uid = ReadUINT16();
		m_packetHeader.reliable = ReadBool();

		if (m_packetHeader.reliable)
		{
			m_packetHeader.counterOut = ReadUINT8();
			m_packetHeader.counterIn = ReadUINT8();
		}
	}

	void WriteDataHeader()
	{
		WriteUINT8(m_dataHeader.lobbyPacketType);

		if (m_packetHeader.reliable)
		{
			WriteUINT16(m_dataHeader.dataSize);
		}
	}

	void ReadDataHeader()
	{
		m_dataHeader.lobbyPacketType = ReadUINT8();

		if (m_packetHeader.reliable)
		{
			m_dataHeader.dataSize = ReadUINT16();
		}
	}

	void WriteError(EDrxLobbyError data)
	{
		WriteUINT16((uint16)data);
	}

	EDrxLobbyError ReadError()
	{
		return (EDrxLobbyError)ReadUINT16();
	}

	void WriteLobbySessionHandle(DrxLobbySessionHandle h)
	{
		WriteUINT8(h.GetID());
	}

	DrxLobbySessionHandle ReadLobbySessionHandle()
	{
		return DrxLobbySessionHandle(ReadUINT8());
	}

	void SetFromSessionSID(uint64 sid)
	{
		m_packetHeader.fromUID.m_sid = sid;
	}

	uint64 GetFromSessionSID()
	{
		return m_packetHeader.fromUID.m_sid;
	}

	void SetFromConnectionUID(SDrxMatchMakingConnectionUID uid)
	{
		m_packetHeader.fromUID = uid;
	}

	SDrxMatchMakingConnectionUID GetFromConnectionUID()
	{
		return m_packetHeader.fromUID;
	}

	SDrxLobbyPacketHeader*     GetLobbyPacketHeader()     { return &m_packetHeader; }
	SDrxLobbyPacketDataHeader* GetLobbyPacketDataHeader() { return &m_dataHeader; }

	void                       ResetBuffer()
	{
		m_allocated = false;
		m_pWriteBuffer = NULL;
	}
};

#endif // __DRXSHAREDLOBBYPACKET_H__
