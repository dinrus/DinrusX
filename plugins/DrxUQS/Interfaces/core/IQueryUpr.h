// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// IQueryUpr
		//
		// - starts queries, updates them over time and allows to prematurely cancel a running query
		// - once a query finishes, it will notify the caller through the callback that was provided by the initial Client::SQueryRequest
		// - for each query, several item-monitors can be installed, and they all will run at least until the end of that query (but may get carried over to the next one in case of hierarchical queries)
		//
		//===================================================================================

		struct IQueryUpr
		{
			virtual                       ~IQueryUpr() {}
			virtual CQueryID              StartQuery(const Client::SQueryRequest& request, Shared::IUqsString& errorMessage) = 0;
			virtual void                  CancelQuery(const CQueryID& idOfQueryToCancel) = 0;
			virtual void                  AddItemMonitorToQuery(const CQueryID& queryID, Client::ItemMonitorUniquePtr&& pItemMonitorToInstall) = 0;
		};

	}
}
