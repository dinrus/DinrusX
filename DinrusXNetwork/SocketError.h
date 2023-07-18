// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SOCKETERROR_H__
#define __SOCKETERROR_H__

#pragma once

#include <DinrusX/DinrusXNetwork/DrxSocketError.h>

ESocketError OSErrorToSocketError(uint32 oserr);

#endif
