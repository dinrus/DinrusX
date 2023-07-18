// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#pragma once

#define eDrxM_3DEngine 1
#define eDrxM_GameFramework 2
#define eDrxM_AISystem 3
#define eDrxM_Animation 4
#define eDrxM_DynamicResponseSystem 5
#define eDrxM_EntitySystem 6
#define eDrxM_Font 7
#define eDrxM_Input 8
#define eDrxM_Movie 9
#define eDrxM_Network 10
// DrxLobby is now an engine plug-in
//#define eDrxM_Lobby 11
#define eDrxM_Physics 12
#define eDrxM_ScriptSystem 13
#define eDrxM_AudioSystem 14
#define eDrxM_System 15
#define eDrxM_LegacyGameDLL 16
#define eDrxM_Render 17
#define eDrxM_Launcher 18
#define eDrxM_Editor 19
#define eDrxM_LiveCreate 20
// DrxOnline has been removed from the engine build for now
//#define eDrxM_Online 21
#define eDrxM_AudioImplementation 22
#define eDrxM_MonoBridge 23
#define eDrxM_ScaleformHelper 24
#define eDrxM_FlowGraph 25
//! Legacy module, does not need a specific name and is only kept for backwards compatibility
#define eDrxM_Legacy 26

#define eDrxM_EnginePlugin 27
#define eDrxM_EditorPlugin 28
#define eDrxM_Schematyc2 29

#define eDrxM_Num 30
#define eDrxM_Game 31

static const wchar_t* g_moduleNames[] =
{
	L"",
	L"DinrusX3dEng",
	L"DinrusXAction",
	L"DinrusXAI",
	L"DinrusXAnimation",
	L"DinrusXDynRespSys",
	L"DinrusXEntity",
	L"DinrusXFont",
	L"DinrusXInput",
	L"DinrusXMovie",
	L"DinrusXNetwork",
	L"DinrusXLobby",
	L"DinrusXPhys",
	L"DinrusXScript",
	L"DinrusXAudio",
	L"DinrusXSys",
	L"DinrusXGame",
	L"DinrusXRenderer",
	L"Launcher",
	L"Sandbox",
	L"DinrusXLiveCreate",
	L"DinrusXOnline",
	L"DinrusXAudioImplementation",
	L"DinrusXMonoBridge",
	L"DinrusXScaleformHelper",
	L"DinrusXFlowGraph",
	L"Legacy Module",
	L"Engine Plug-ins",
	L"Editor Plug-ins",
	L"Schematyc2"
};