// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//////////////////////////////////////////////////////////////////////////
		// CItemSerializationSupport 
		//////////////////////////////////////////////////////////////////////////

		class CItemSerializationSupport : public IItemSerializationSupport
		{
		public:
			// IItemSerializationSupport
			virtual bool                     DeserializeItemFromCStringLiteral(void* pOutItem, const Client::IItemFactory& itemFactory, const char* szItemLiteral, Shared::IUqsString* pErrorMessage) const override;
			virtual bool                     DeserializeItemIntoDictFromCStringLiteral(Shared::IVariantDict& out, const char* szKey, Client::IItemFactory& itemFactory, const char* szItemLiteral, Shared::IUqsString* pErrorMessage) const override;
			virtual bool                     SerializeItemToStringLiteral(const void* pItem, const Client::IItemFactory& itemFactory, Shared::IUqsString& outString) const override;
			// ~IItemSerializationSupport
		};

	}
}
