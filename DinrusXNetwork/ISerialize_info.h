// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXNetwork/ISerialize.h> // <> required for Interfuscator

STRUCT_INFO_BEGIN(SNetObjectID)
STRUCT_VAR_INFO(id, TYPE_INFO(uint16))
STRUCT_VAR_INFO(salt, TYPE_INFO(uint16))
STRUCT_INFO_END(SNetObjectID)

STRUCT_INFO_EMPTY(SSerializeString)
