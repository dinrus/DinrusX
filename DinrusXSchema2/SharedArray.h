// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

namespace Schematyc2
{
	template <typename TYPE> class CSharedArray
	{
	private:

		typedef DynArray<TYPE, uint32> Array;
		typedef std::shared_ptr<Array> ArrayPtr;

	public:

		CSharedArray() {}

		CSharedArray(const CSharedArray& rhs)
			: m_pArray(rhs.m_pArray)
		{}

		uint32 GetSize() const
		{
			return m_pArray ? m_pArray->size() : 0;
		}

		void PushBack(const TYPE& value)
		{
			if(!m_pArray)
			{
				m_pArray = std::make_shared<Array>(*m_pArray);
			}
			MakeUnique();
			m_pArray->push_back(value);
		}

		void PopBack()
		{
			DRX_ASSERT(m_pArray);
			MakeUnique();
			m_pArray->pop_back();
		}

		void SetElement(uint32 idx, const TYPE& value) const
		{
			DRX_ASSERT(m_pArray);
			MakeUnique();
			(*m_pArray)[idx] = value;
		}

		const TYPE& GetElement(uint32 idx) const
		{
			DRX_ASSERT(m_pArray);
			return (*m_pArray)[idx];
		}

	private:

		void MakeUnique()
		{
			if(!m_pArray.unique())
			{
				m_pArray = std::make_shared<Array>(*m_pArray);
			}
		}

	private:

		ArrayPtr m_pArray;
	};
}
