// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Client
	{
		namespace Internal
		{

			//===================================================================================
			//
			// CParamsHolder<>
			//
			//===================================================================================

			template <class TParams>
			class CParamsHolder : public IParamsHolder
			{
			public:
				virtual void*    GetParams() override;

			private:
				TParams          m_params;
			};

			template <class TParams>
			void* CParamsHolder<TParams>::GetParams()
			{
				return &m_params;
			}

		}
	}
}
