// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: System for managing storage and retrieval of shared parameters.

   -------------------------------------------------------------------------
   History:
   - 29:04:2010: Created by Paul Slinger

*************************************************************************/

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/SharedParamsUpr.h>

CSharedParamsUpr::CSharedParamsUpr() : m_sizeOfSharedParams(0), m_lock(true), m_outputLockWarning(true)
{
}

CSharedParamsUpr::~CSharedParamsUpr()
{
}

uint32 CSharedParamsUpr::GenerateCRC32(const char* pName)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Check name.

		if (pName)
		{
			// Check to see if name has already been registered.

			uint32 crc32 = CCrc32::Compute(pName);

			TNameMap::const_iterator iName = m_names.find(crc32);

			if (iName == m_names.end())
			{
				// Register name.

				m_names.insert(TNameMap::value_type(crc32, pName));

				return crc32;
			}
			else if (iName->second == pName)
			{
				return crc32;
			}
		}
	}

	return 0;
}

ISharedParamsConstPtr CSharedParamsUpr::Register(uint32 crc32, const ISharedParams& sharedParams)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Check name has been registered.

		if (m_names.find(crc32) != m_names.end())
		{
			// Are we running the editor? If so don't register shared parameters.

			if (gEnv->IsEditor())
			{
				return ISharedParamsConstPtr(sharedParams.Clone());
			}
			else
			{
				// Check to see if record already exists.

				if (m_records.find(crc32) == m_records.end())
				{
					// Clone shared parameters.

					ISharedParamsPtr pSharedParams(sharedParams.Clone());

					if (pSharedParams)
					{
						// Store record.

						m_records.insert(TRecordMap::value_type(crc32, pSharedParams));

						m_sizeOfSharedParams += pSharedParams->GetTypeInfo().GetSize();

						return pSharedParams;
					}
				}
			}
		}
	}

	return ISharedParamsConstPtr();
}

ISharedParamsConstPtr CSharedParamsUpr::Register(const char* pName, const ISharedParams& sharedParams)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Generate 32 bit crc.

		uint32 crc32 = GenerateCRC32(pName);

		if (crc32)
		{
			// Register shared parameters.

			return Register(crc32, sharedParams);
		}
	}

	return ISharedParamsConstPtr();
}

void CSharedParamsUpr::Remove(uint32 crc32)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Get record.

		TRecordMap::iterator iRecord = m_records.find(crc32);

		if (iRecord != m_records.end())
		{
			// Remove record.

			m_records.erase(iRecord);
		}
	}
}

void CSharedParamsUpr::Remove(const char* pName)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Check name.

		if (pName)
		{
			uint32 crc32 = CCrc32::Compute(pName);

			TNameMap::const_iterator iName = m_names.find(crc32);

			if (iName != m_names.end() && iName->second == pName)
			{
				// Remove record.

				Remove(crc32);
			}
		}
	}
}

void CSharedParamsUpr::RemoveByType(const CSharedParamsTypeInfo& typeInfo)
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Remove records by type.

		for (TRecordMap::iterator iRecord = m_records.begin(), iEndRecord = m_records.end(); iRecord != iEndRecord; )
		{
			const ISharedParamsPtr& pSharedParams = iRecord->second;

			if (pSharedParams->GetTypeInfo() == typeInfo)
			{
				TRecordMap::iterator iEraseRecord = iRecord;

				++iRecord;

				m_records.erase(iEraseRecord);
			}
			else
			{
				++iRecord;
			}
		}
	}
}

ISharedParamsConstPtr CSharedParamsUpr::Get(uint32 crc32) const
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Get record.

		TRecordMap::const_iterator iRecord = m_records.find(crc32);

		if (iRecord != m_records.end())
		{
			return iRecord->second;
		}
	}

	return ISharedParamsConstPtr();
}

ISharedParamsConstPtr CSharedParamsUpr::Get(const char* pName) const
{
	// Check lock.

	if (VerifyUnlocked())
	{
		// Check name.

		if (pName)
		{
			uint32 crc32 = CCrc32::Compute(pName);

			TNameMap::const_iterator iName = m_names.find(crc32);

			if (iName != m_names.end() && iName->second == pName)
			{
				// Get record.

				return Get(crc32);
			}
		}
	}

	return ISharedParamsConstPtr();
}

void CSharedParamsUpr::Reset()
{
#ifdef _DEBUG
	for (TRecordMap::const_iterator iRecord = m_records.begin(), iEndRecord = m_records.end(); iRecord != iEndRecord; ++iRecord)
	{
		long use_count = iRecord->second.use_count();

		DRX_ASSERT_TRACE(use_count == 1, ("Shared params structure '%s' is still referenced in %d place%s", m_names[iRecord->first].c_str(), use_count - 1, (use_count == 2) ? "" : "s"));
	}
#endif //_DEBUG

	// Ensure all memory is returned to the level heap.

	stl::free_container(m_records);

	stl::free_container(m_names);

	m_sizeOfSharedParams = 0;
}

void CSharedParamsUpr::Release()
{
	delete this;
}

void CSharedParamsUpr::GetMemoryStatistics(IDrxSizer* pSizer)
{
	DRX_ASSERT(pSizer);

	pSizer->Add(*this);

	pSizer->AddContainer(m_names);

	pSizer->AddContainer(m_records);

	pSizer->AddObject(&m_sizeOfSharedParams, m_sizeOfSharedParams);
}

void CSharedParamsUpr::Lock()
{
	m_lock = true;
}

void CSharedParamsUpr::Unlock()
{
	m_lock = false;
}

bool CSharedParamsUpr::VerifyUnlocked() const
{
	DRX_ASSERT(!m_lock);

	if (m_lock && m_outputLockWarning)
	{
		DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CSharedParamsUpr::VerifyUnlocked: Attempting to access shared parameters whilst manager is locked");

		m_outputLockWarning = false;
	}

	return !m_lock;
}
