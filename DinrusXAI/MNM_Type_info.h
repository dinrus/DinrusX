// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/CoreX/TypeInfo_impl.h>

#include <DinrusX/DinrusXAI/MNMFixedAABB.h>
#include <DinrusX/DinrusXAI/MNMFixedVec2.h>
#include <DinrusX/DinrusXAI/MNMFixedVec3.h>

STRUCT_INFO_T2_BEGIN(fixed_t, typename, BaseType, size_t, IntegerBitCount)
VAR_INFO(v)
STRUCT_INFO_T2_END(fixed_t, typename, BaseType, size_t, size_t)

STRUCT_INFO_T2_BEGIN(MNM::FixedAABB, typename, BaseType, size_t, IntegerBitCount)
VAR_INFO(min)
VAR_INFO(max)
STRUCT_INFO_T2_END(MNM::FixedAABB, typename, BaseType, size_t, size_t)

STRUCT_INFO_T2_BEGIN(MNM::FixedVec3, typename, BaseType, size_t, IntegerBitCount)
VAR_INFO(x)
VAR_INFO(y)
VAR_INFO(z)
STRUCT_INFO_T2_END(MNM::FixedVec3, typename, BaseType, size_t, size_t)
