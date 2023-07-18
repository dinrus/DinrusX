// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
DrxWatch.cpp

Description: 
- basic onscreen watch
- in game.dll till it matures and can be moved into the engine

-------------------------------------------------------------------------
History:
-	[03/08/2009] : Created by James Bamford

*************************************************************************/

#ifndef __DRXWATCH_H__
#define __DRXWATCH_H__

#if !defined(_RELEASE)
#define DRX_WATCH_ENABLED			 (1)
#else
#define DRX_WATCH_ENABLED			 (0)
#endif

#define DrxWatch(...) DrxWatchFunc(string().Format(__VA_ARGS__))
#define DrxWatchLog(...) DrxWatchLogFunc(string().Format(__VA_ARGS__))

#if DRX_WATCH_ENABLED

int DrxWatchFunc(const char * message);
int DrxWatchLogFunc(const char * message);
void DrxWatch3DAdd(const char * text, const Vec3 & posIn, float lifetime = 2.f, const Vec3 * velocity = NULL, float gravity = 3.f);
void DrxWatch3DReset();
void DrxWatch3DTick(float dt);

#else

#define DrxWatchFunc(message)          (0)
#define DrxWatchLogFunc(message)          (0)
#define DrxWatch3DAdd(...)             ((void)0)
#define DrxWatch3DReset()              ((void)0)
#define DrxWatch3DTick(dt)             ((void)0)

#endif // DRX_WATCH_ENABLED

#endif // __DRXWATCH_H__