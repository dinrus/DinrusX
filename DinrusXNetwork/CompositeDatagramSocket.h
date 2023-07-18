// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#include <DinrusX/DinrusXNetwork/IDatagramSocket.h>

class CCompositeDatagramSocket : public IDatagramSocket
{
public:
	CCompositeDatagramSocket();
	~CCompositeDatagramSocket();

	void AddChild(IDatagramSocketPtr);

	// IDatagramSocketPtr
	virtual void         GetSocketAddresses(TNetAddressVec& addrs) override;
	virtual ESocketError Send(const uint8* pBuffer, size_t nLength, const TNetAddress& to) override;
	virtual ESocketError SendVoice(const uint8* pBuffer, size_t nLength, const TNetAddress& to) override;
	virtual void         RegisterListener(IDatagramListener* pListener) override;
	virtual void         UnregisterListener(IDatagramListener* pListener) override;
	virtual void         Die() override;
	virtual bool         IsDead() override;
	virtual DRXSOCKET    GetSysSocket() override;
	virtual void         RegisterBackoffAddress(const TNetAddress& addr) override;
	virtual void         UnregisterBackoffAddress(const TNetAddress& addr) override;
	// ~IDatagramSocketPtr

private:
	typedef std::vector<IDatagramSocketPtr> ChildVec;
	typedef ChildVec::iterator              ChildVecIter;
	ChildVec m_children;
};
