// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Manages one-off reader/writer usages for binary serialization

   -------------------------------------------------------------------------
   History:
   - 02:06:2010: Created by Kevin Kirst

*************************************************************************/

#ifndef __BINARYREADERWRITER_H__
#define __BINARYREADERWRITER_H__

#include <DinrusX/DinrusXNetwork/ISerializeHelper.h>

#include <DinrusX/DinrusXAct/SerializeWriterXMLCPBin.h>
#include <DinrusX/DinrusXAct/SerializeReaderXMLCPBin.h>
#include <DinrusX/DinrusXAct/XMLCPB_WriterInterface.h>
#include <DinrusX/DinrusXAct/XMLCPB_ReaderInterface.h>

class CBinarySerializedObject : public ISerializedObject
{
public:
	CBinarySerializedObject(const char* szSection);
	virtual ~CBinarySerializedObject();

	enum { GUID = 0xBDE84A9A };
	virtual uint32 GetGUID() const { return GUID; }
	virtual void   GetMemoryUsage(IDrxSizer* pSizer) const;

	virtual void   AddRef()  { ++m_nRefCount; }
	virtual void   Release() { if (--m_nRefCount <= 0) delete this; }

	virtual bool   IsEmpty() const;
	virtual void   Reset();
	virtual void   Serialize(TSerialize& serialize);

	bool           FinishWriting(XMLCPB::CWriterInterface& Writer);
	bool           PrepareReading(XMLCPB::CReaderInterface& Reader);
	const char*    GetSectionName() const { return m_sSection.c_str(); }

private:
	void FreeData();

	string m_sSection;

	int    m_nRefCount;
	uint32 m_uSerializedDataSize;
	uint8* m_pSerializedData;
};

class CBinarySerializeHelper : public ISerializeHelper
{
public:
	CBinarySerializeHelper();
	virtual ~CBinarySerializeHelper();

	virtual void                          GetMemoryUsage(IDrxSizer* pSizer) const;

	virtual void                          AddRef()  { ++m_nRefCount; }
	virtual void                          Release() { if (--m_nRefCount <= 0) delete this; }

	virtual _smart_ptr<ISerializedObject> CreateSerializedObject(const char* szSection);
	virtual bool                          Write(ISerializedObject* pObject, TSerializeFunc serializeFunc, void* pArgument = NULL);
	virtual bool                          Read(ISerializedObject* pObject, TSerializeFunc serializeFunc, void* pArgument = NULL);

private:
	static CBinarySerializedObject* GetBinarySerializedObject(ISerializedObject* pObject);

private:
	int m_nRefCount;
};

#endif //__BINARYREADERWRITER_H__
