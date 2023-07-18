// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __FLOWSERIALIZE_H__
#define __FLOWSERIALIZE_H__

#pragma once

#include <DinrusX/DinrusXFlowGraph/IFlowSystem.h>

bool   SetFromString(TFlowInputData& value, const char* str);
string ConvertToString(const TFlowInputData& value);
bool   SetAttr(XmlNodeRef node, const char* attr, const TFlowInputData& value);

#endif
