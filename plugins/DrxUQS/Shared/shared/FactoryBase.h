// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Shared
	{

		//===================================================================================
		//
		// CFactoryBase<>
		//
		//===================================================================================

		template <class TFactory>		// to be used as the curiously recurring template pattern
		class CFactoryBase
		{
		public:
			template <class TFactoryInterface>
			static void        RegisterAllInstancesInFactoryDatabase(Core::IFactoryDatabase<TFactoryInterface>& databaseToRegisterInstancesIn);

		protected:
			explicit           CFactoryBase(const char* szName, const DrxGUID& guid);
			const char*        GetName() const;
			const DrxGUID&     GetGUID() const;

		private:
			                   UQS_NON_COPYABLE(CFactoryBase);

		private:
			string             m_name;
			DrxGUID            m_guid;
			TFactory*          m_pNext;
			static TFactory*   s_pList;
		};

		template <class TFactory>
		TFactory* CFactoryBase<TFactory>::s_pList;

		template <class TFactory>
		CFactoryBase<TFactory>::CFactoryBase(const char* szName, const DrxGUID& guid)
			: m_name(szName)
			, m_guid(guid)
		{
			// notice: we silently accept possible duplicates here (the factory-database will do such consistency checks)
			m_pNext = s_pList;
			s_pList = static_cast<TFactory*>(this);
		}

		template <class TFactory>
		const char* CFactoryBase<TFactory>::GetName() const
		{
			return m_name.c_str();
		}

		template <class TFactory>
		const DrxGUID& CFactoryBase<TFactory>::GetGUID() const
		{
			return m_guid;
		}

		template <class TFactory>
		template <class TFactoryInterface>
		void CFactoryBase<TFactory>::RegisterAllInstancesInFactoryDatabase(Core::IFactoryDatabase<TFactoryInterface>& databaseToRegisterInstancesIn)
		{
			for (TFactory* pCur = s_pList; pCur; pCur = pCur->m_pNext)
			{
				databaseToRegisterInstancesIn.RegisterFactory(pCur, pCur->m_name.c_str(), pCur->m_guid);
			}
		}

	}
}
