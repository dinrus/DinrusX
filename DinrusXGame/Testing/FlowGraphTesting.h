// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
History:
- 13:05:2009   Created by Federico Rebora
*************************************************************************/

#pragma once

#ifndef FLOW_GRAPH_TESTING_H_INCLUDED
#define FLOW_GRAPH_TESTING_H_INCLUDED

#include <DrxFlowGraph/IFlowSystem.h>

DrxUnit::StringStream& operator << (DrxUnit::StringStream& stringStream, const SInputPortConfig& portConfig);

namespace GameTesting
{
	class CFlowNodeTestingFacility
	{
	public:
		CFlowNodeTestingFacility(IFlowNode& nodeToTest, const unsigned int inputPortsCount);
		~CFlowNodeTestingFacility();

		void ProcessEvent(IFlowNode::EFlowEvent event);

		template <class InputType>
		void SetInputByIndex(const unsigned int inputIndex, const InputType& value)
		{
			m_inputData[inputIndex].Set(value);
		}

	private:
		IFlowNode& m_nodeToTest;
		TFlowInputData* m_inputData;
	};
}

#endif //FLOW_GRAPH_TESTING_H_INCLUDED
