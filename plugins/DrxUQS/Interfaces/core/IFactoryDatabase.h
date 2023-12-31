// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// IFactoryDatabase<>
		//
		//===================================================================================

		template <class TFactory>
		struct IFactoryDatabase
		{
			virtual               ~IFactoryDatabase() {}
			virtual void          RegisterFactory(TFactory* pFactoryToRegister, const char* szName, const DrxGUID& guid) = 0;
			virtual TFactory*     FindFactoryByName(const char* szName) const = 0;
			virtual TFactory*     FindFactoryByGUID(const DrxGUID& guid) const = 0;
			virtual TFactory*     FindFactoryByCallback(const std::function<bool(const TFactory&)>& callback) const = 0;
			virtual size_t        GetFactoryCount() const = 0;
			virtual TFactory&     GetFactory(size_t index) const = 0;
		};

	}
}
