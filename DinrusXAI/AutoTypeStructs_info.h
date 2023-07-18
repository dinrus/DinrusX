// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/AutoTypeStructs.h>

STRUCT_INFO_BEGIN(SLinkRecord)
STRUCT_VAR_INFO(nodeIndex, TYPE_INFO(int))
STRUCT_VAR_INFO(radiusOut, TYPE_INFO(float))
STRUCT_VAR_INFO(radiusIn, TYPE_INFO(float))
STRUCT_INFO_END(SLinkRecord)

STRUCT_INFO_BEGIN(SFlightLinkDesc)
STRUCT_VAR_INFO(index1, TYPE_INFO(int))
STRUCT_VAR_INFO(index2, TYPE_INFO(int))
STRUCT_INFO_END(SFlightLinkDesc)

STRUCT_INFO_BEGIN(NodeDescriptor)
STRUCT_VAR_INFO(ID, TYPE_INFO(unsigned int))
STRUCT_VAR_INFO(dir, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(up, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(pos, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(index, TYPE_INFO(int))
STRUCT_VAR_INFO(obstacle, TYPE_ARRAY(3, TYPE_INFO(int)))
STRUCT_VAR_INFO(navType, TYPE_INFO(uint16))
STRUCT_BITFIELD_INFO(type, uint8, 4)
STRUCT_BITFIELD_INFO(bForbidden, uint8, 1)
STRUCT_BITFIELD_INFO(bForbiddenDesigner, uint8, 1)
STRUCT_BITFIELD_INFO(bRemovable, uint8, 1)
STRUCT_INFO_END(NodeDescriptor)

STRUCT_INFO_BEGIN(LinkDescriptor)
STRUCT_VAR_INFO(nSourceNode, TYPE_INFO(unsigned int))
STRUCT_VAR_INFO(nTargetNode, TYPE_INFO(unsigned int))
STRUCT_VAR_INFO(vEdgeCenter, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(fMaxPassRadius, TYPE_INFO(float))
STRUCT_VAR_INFO(fExposure, TYPE_INFO(float))
STRUCT_VAR_INFO(fLength, TYPE_INFO(float))
STRUCT_VAR_INFO(fMaxWaterDepth, TYPE_INFO(float))
STRUCT_VAR_INFO(fMinWaterDepth, TYPE_INFO(float))
STRUCT_VAR_INFO(nStartIndex, TYPE_INFO(uint8))
STRUCT_VAR_INFO(nEndIndex, TYPE_INFO(uint8))
STRUCT_BITFIELD_INFO(bIsPureTriangularLink, uint8, 1)
STRUCT_BITFIELD_INFO(bSimplePassabilityCheck, uint8, 1)
STRUCT_INFO_END(LinkDescriptor)

STRUCT_INFO_BEGIN(SVolumeHideSpot)
STRUCT_VAR_INFO(pos, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(dir, TYPE_INFO(Vec3))
STRUCT_INFO_END(SVolumeHideSpot)

STRUCT_INFO_BEGIN(ObstacleDataDesc)
STRUCT_VAR_INFO(vPos, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(vDir, TYPE_INFO(Vec3))
STRUCT_VAR_INFO(fApproxRadius, TYPE_INFO(float))
STRUCT_VAR_INFO(flags, TYPE_INFO(uint8))
STRUCT_VAR_INFO(approxHeight, TYPE_INFO(uint8))
STRUCT_INFO_END(ObstacleDataDesc)

STRUCT_INFO_BEGIN(SFlightDesc)
STRUCT_VAR_INFO(x, TYPE_INFO(float))
STRUCT_VAR_INFO(y, TYPE_INFO(float))
STRUCT_VAR_INFO(minz, TYPE_INFO(float))
STRUCT_VAR_INFO(maxz, TYPE_INFO(float))
STRUCT_VAR_INFO(maxRadius, TYPE_INFO(float))
STRUCT_VAR_INFO(classification, TYPE_INFO(int))
STRUCT_VAR_INFO(childIdx, TYPE_INFO(int))
STRUCT_VAR_INFO(nextIdx, TYPE_INFO(int))
STRUCT_INFO_END(SFlightDesc)