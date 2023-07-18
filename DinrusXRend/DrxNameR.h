// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxNameR.h
//  Version:     v1.00
//  Created:     12/04/2010 by Andrey.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DrxNameR_h__
#define __DrxNameR_h__
#pragma once

#include <DinrusX/CoreX/DrxCrc32.h>

//#define CHECK_INVALID_ACCESS

//////////////////////////////////////////////////////////////////////////
class CNameTableR
{
public:
	// Name entry header, immediately after this header in memory starts actual string data.
	struct SNameEntryR
	{
		// Reference count of this string.
		int nRefCount;
		// Current length of string.
		int nLength;
		// Size of memory allocated at the end of this class.
		int nAllocSize;
		// Here in memory starts character buffer of size nAllocSize.
		//char data[nAllocSize]

		const char* GetStr()         { return (char*)(this + 1); }
		void        AddRef()         { DrxInterlockedIncrement(&nRefCount); };
		int         Release()        { return DrxInterlockedDecrement(&nRefCount); };
		int         GetMemoryUsage() { return sizeof(SNameEntryR) + strlen(GetStr()); }
		int         GetLength()      { return nLength; }
	};

	static threadID m_nRenderThread;

private:
	typedef std::unordered_map<const char*, SNameEntryR*, stl::hash_stricmp<const char*>, stl::hash_stricmp<const char*>> NameMap;
	NameMap m_nameMap;

	void CheckThread()
	{
#ifdef CHECK_INVALID_ACCESS
		DWORD d = ::GetCurrentThreadId();
		if (m_nRenderThread != 0 && d != m_nRenderThread)
		{
			__debugbreak();
		}
#endif
	}

public:

	CNameTableR() {}

	~CNameTableR()
	{
		for (NameMap::iterator it = m_nameMap.begin(); it != m_nameMap.end(); ++it)
		{
			free(it->second);
		}
	}

	// Only finds an existing name table entry, return 0 if not found.
	SNameEntryR* FindEntry(const char* str)
	{
		CheckThread();
		SNameEntryR* pEntry = stl::find_in_map(m_nameMap, str, 0);
		return pEntry;
	}

	// Finds an existing name table entry, or creates a new one if not found.
	SNameEntryR* GetEntry(const char* str)
	{
		CheckThread();
		SNameEntryR* pEntry = stl::find_in_map(m_nameMap, str, 0);
		if (!pEntry)
		{
			// Create a new entry.
			unsigned int nLen = strlen(str);
			unsigned int allocLen = sizeof(SNameEntryR) + (nLen + 1) * sizeof(char);
			pEntry = (SNameEntryR*)malloc(allocLen);
			assert(pEntry != NULL);
			pEntry->nRefCount = 0;
			pEntry->nLength = nLen;
			pEntry->nAllocSize = allocLen;
			// Copy string to the end of name entry.
			char* pEntryStr = const_cast<char*>(pEntry->GetStr());
			memcpy(pEntryStr, str, nLen + 1);
			// put in map.
			//m_nameMap.insert( NameMap::value_type(pEntry->GetStr(),pEntry) );
			m_nameMap[pEntry->GetStr()] = pEntry;
		}
		return pEntry;
	}

	// Release existing name table entry.
	void Release(SNameEntryR* pEntry)
	{
		CheckThread();
		assert(pEntry);
		m_nameMap.erase(pEntry->GetStr());
		free(pEntry);
	}
	int GetMemoryUsage()
	{
		int nSize = 0;
		NameMap::iterator it;
		int n = 0;
		for (it = m_nameMap.begin(); it != m_nameMap.end(); ++it)
		{
			nSize += strlen(it->first);
			nSize += it->second->GetMemoryUsage();
			n++;
		}
		nSize += n * 8;

		return nSize;
	}
	void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_nameMap);
	}
	int GetNumberOfEntries()
	{
		return m_nameMap.size();
	}

	// Log all names inside DrxNameTS table.
	void LogNames()
	{
		NameMap::iterator it;
		for (it = m_nameMap.begin(); it != m_nameMap.end(); ++it)
		{
			SNameEntryR* pNameEntry = it->second;
			DrxLog("[%4d] %s", pNameEntry->nLength, pNameEntry->GetStr());
		}
	}

};

//////////////////////////////////////////////////////////////////////////
// Class CDrxNameR
//////////////////////////////////////////////////////////////////////////
class CDrxNameR
{
public:
	CDrxNameR();
	CDrxNameR(const CDrxNameR& n);
	// !!! do not allow implicit conversion as it can lead to subtle bugs when passing const char* values
	// to stl algorithms (as operator < will create a CDrxNameR and potentially insert into the name table
	// while processing the algorithm)
	explicit CDrxNameR(const char* s);
	~CDrxNameR();

	CDrxNameR& operator=(const CDrxNameR& n);
	CDrxNameR& operator=(const char* s);

	bool       operator==(const CDrxNameR& n) const;
	bool       operator<(const CDrxNameR& n) const;

	bool       empty() const
	{
		return length() == 0;
	}
	void reset()
	{
		_release(m_str);
		m_str = 0;
	}
	void addref()
	{
		_addref(m_str);
	}

	const char* c_str() const
	{
		return (m_str) ? m_str : "";
	}
	int length() const
	{
		return _length();
	};

	static bool find(const char* str)
	{
		return GetNameTable()->FindEntry(str) != 0;
	}
	void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		//pSizer->AddObject(m_str);
		pSizer->AddObject(GetNameTable());
	}
	static int GetMemoryUsage()
	{
		CNameTableR* pTable = GetNameTable();
		return pTable->GetMemoryUsage();
	}
	static int GetNumberOfEntries()
	{
		CNameTableR* pTable = GetNameTable();
		return pTable->GetNumberOfEntries();
	}

private:
	typedef CNameTableR::SNameEntryR SNameEntry;
	static CNameTableR* ms_table;

	static CNameTableR* GetNameTable()
	{
		// Note: can not use a 'static CNameTable sTable' here, because that
		// implies a static destruction order dependency - the name table is
		// accessed from static destructor calls.

		if (ms_table == NULL)
			ms_table = new CNameTableR();
		return ms_table;
	}

	SNameEntry* _entry(const char* pBuffer) const
	{
		assert(pBuffer);
		return ((SNameEntry*)pBuffer) - 1;
	}
	void _release(const char* pBuffer)
	{
		if (pBuffer && _entry(pBuffer)->Release() <= 0)
			GetNameTable()->Release(_entry(pBuffer));
	}
	int _length() const
	{
		return (m_str) ? _entry(m_str)->nLength : 0;
	}
	void _addref(const char* pBuffer)
	{
		if (pBuffer)
			_entry(pBuffer)->AddRef();
	}

	const char* m_str;
};

inline CDrxNameR::CDrxNameR()
{
	m_str = 0;
}

inline CDrxNameR::CDrxNameR(const CDrxNameR& n)
{
	_addref(n.m_str);
	m_str = n.m_str;
}

inline CDrxNameR::CDrxNameR(const char* s)
{
	const char* pBuf = 0;
	if (s && *s)
		pBuf = GetNameTable()->GetEntry(s)->GetStr();

	_addref(pBuf);
	m_str = pBuf;
}

inline CDrxNameR::~CDrxNameR()
{
	_release(m_str);
}

inline CDrxNameR& CDrxNameR::operator=(const CDrxNameR& n)
{
	_addref(n.m_str);
	_release(m_str);
	m_str = n.m_str;
	return *this;
}

inline CDrxNameR& CDrxNameR::operator=(const char* s)
{
	const char* pBuf = 0;
	if (s && *s)
		pBuf = GetNameTable()->GetEntry(s)->GetStr();

	_addref(pBuf);
	_release(m_str);
	m_str = pBuf;
	return *this;
}

inline bool CDrxNameR::operator==(const CDrxNameR& n) const
{
	return m_str == n.m_str;
}

inline bool CDrxNameR::operator<(const CDrxNameR& n) const
{
	return m_str < n.m_str;
}

///////////////////////////////////////////////////////////////////////////////
// Class CDrxNameTSCRC.
//////////////////////////////////////////////////////////////////////////
class CDrxNameTSCRC
{
public:
	CDrxNameTSCRC();
	CDrxNameTSCRC(const CDrxNameTSCRC& n);
	CDrxNameTSCRC(const char* s);
	CDrxNameTSCRC(const char* s, bool bOnlyFind);
	CDrxNameTSCRC(uint32 n) { m_nID = n; }
	~CDrxNameTSCRC();

	CDrxNameTSCRC& operator=(const CDrxNameTSCRC& n);
	CDrxNameTSCRC& operator=(const char* s);

	bool           operator==(const CDrxNameTSCRC& n) const;
	bool           operator!=(const CDrxNameTSCRC& n) const;

	bool           operator==(const char* s) const;
	bool           operator!=(const char* s) const;

	bool           operator<(const CDrxNameTSCRC& n) const;
	bool           operator>(const CDrxNameTSCRC& n) const;

	bool           empty() const { return m_nID == 0; }
	void           reset()       { m_nID = 0; }
	uint32         get() const   { return m_nID; }
	void           add(int nAdd) { m_nID += nAdd; }

	AUTO_STRUCT_INFO;

	void GetMemoryUsage(IDrxSizer* pSizer) const { /*nothing*/ }
private:

	uint32 m_nID;

};

//////////////////////////////////////////////////////////////////////////
// CDrxNameTSCRC
//////////////////////////////////////////////////////////////////////////
inline CDrxNameTSCRC::CDrxNameTSCRC()
{
	m_nID = 0;
}

//////////////////////////////////////////////////////////////////////////
inline CDrxNameTSCRC::CDrxNameTSCRC(const CDrxNameTSCRC& n)
{
	m_nID = n.m_nID;
}

//////////////////////////////////////////////////////////////////////////
inline CDrxNameTSCRC::CDrxNameTSCRC(const char* s)
{
	m_nID = 0;
	*this = s;
}

inline CDrxNameTSCRC::~CDrxNameTSCRC()
{
	m_nID = 0;
}

//////////////////////////////////////////////////////////////////////////
inline CDrxNameTSCRC& CDrxNameTSCRC::operator=(const CDrxNameTSCRC& n)
{
	m_nID = n.m_nID;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline CDrxNameTSCRC& CDrxNameTSCRC::operator=(const char* s)
{
	assert(s);
	if (*s) // if not empty
	{
		m_nID = CCrc32::ComputeLowercase(s);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline bool CDrxNameTSCRC::operator==(const CDrxNameTSCRC& n) const
{
	return m_nID == n.m_nID;
}

inline bool CDrxNameTSCRC::operator!=(const CDrxNameTSCRC& n) const
{
	return !(*this == n);
}

inline bool CDrxNameTSCRC::operator==(const char* str) const
{
	assert(str);
	if (*str) // if not empty
	{
		uint32 nID = CCrc32::ComputeLowercase(str);
		return m_nID == nID;
	}
	return m_nID == 0;
}

inline bool CDrxNameTSCRC::operator!=(const char* str) const
{
	if (!m_nID)
		return true;
	if (*str) // if not empty
	{
		uint32 nID = CCrc32::ComputeLowercase(str);
		return m_nID != nID;
	}
	return false;
}

inline bool CDrxNameTSCRC::operator<(const CDrxNameTSCRC& n) const
{
	return m_nID < n.m_nID;
}

inline bool CDrxNameTSCRC::operator>(const CDrxNameTSCRC& n) const
{
	return m_nID > n.m_nID;
}

inline bool operator==(const string& s, const CDrxNameTSCRC& n)
{
	return n == s;
}
inline bool operator!=(const string& s, const CDrxNameTSCRC& n)
{
	return n != s;
}

inline bool operator==(const char* s, const CDrxNameTSCRC& n)
{
	return n == s;
}
inline bool operator!=(const char* s, const CDrxNameTSCRC& n)
{
	return n != s;
}

#endif //__DrxNameTS_h__
