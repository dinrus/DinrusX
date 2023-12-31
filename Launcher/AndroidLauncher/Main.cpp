// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"

#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/CoreX/Platform/AndroidSpecific.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#define LOG_TAG "Cry"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#if !defined(DEDICATED_SERVER)
#include <SDL.h>
#include <SDL_Extension.h>
#endif

#include <CryGame/IGameStartup.h>
#include <DinrusX/DinrusXEntity/IEntity.h>
#include <CryGame/IGameFramework.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/resource.h>
#include <sys/prctl.h>

#include <DinrusX/DinrusXSys/ParseEngineConfig.h>

#if defined(_LIB)
#include <DinrusX/CoreX/Common_TypeInfo.h>
STRUCT_INFO_T_INSTANTIATE(Vec2_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Vec2_tpl, <int>)
STRUCT_INFO_T_INSTANTIATE(Vec4_tpl, <short>)
STRUCT_INFO_T_INSTANTIATE(Vec3_tpl, <int>)
STRUCT_INFO_T_INSTANTIATE(Vec3_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Ang3_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Quat_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(QuatT_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Plane_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Matrix33_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Color_tpl, <float>)
STRUCT_INFO_T_INSTANTIATE(Color_tpl, <uint8>)

#endif

size_t linux_autoload_level_maxsize = PATH_MAX;
char linux_autoload_level_buf[PATH_MAX];
char *linux_autoload_level = linux_autoload_level_buf;


bool GetDefaultThreadStackSize(size_t* pStackSize)
{
	pthread_attr_t kDefAttr;
	pthread_attr_init(&kDefAttr);   // Required on Mac OS or pthread_attr_getstacksize will fail
	int iRes(pthread_attr_getstacksize(&kDefAttr, pStackSize));
	if (iRes != 0)
	{
		fprintf(stderr, "error: pthread_attr_getstacksize returned %d\n", iRes);
		return false;
	}
	return true;
}


bool IncreaseResourceMaxLimit(int iResource, rlim_t uMax)
{
	struct rlimit kLimit;
	if (getrlimit(iResource, &kLimit) != 0)
	{
		fprintf(stderr, "error: getrlimit (%d) failed\n", iResource);
		return false;
	}

	if (uMax != kLimit.rlim_max)
	{
		//if (uMax == RLIM_INFINITY || uMax > kLimit.rlim_max)
		{
			kLimit.rlim_max = uMax;
			if (setrlimit(iResource, &kLimit) != 0)
			{
				fprintf(stderr, "error: setrlimit (%d, %lu) failed\n", iResource, uMax);
				return false;
			}
		}
	}

	return true;
}

#if defined(_LIB)
extern "C" DLL_IMPORT IGameStartup* CreateGameStartup();
#endif

size_t fopenwrapper_basedir_maxsize = MAX_PATH;
namespace { char fopenwrapper_basedir_buffer[MAX_PATH] = ""; }
char * fopenwrapper_basedir = fopenwrapper_basedir_buffer;
bool fopenwrapper_trace_fopen = false;

#define RunGame_EXIT(exitCode) (exit(exitCode))

#define LINUX_LAUNCHER_CONF "launcher.cfg"

static void strip(char *s)
{
	char *p = s, *p_end = s + strlen(s);

	while (*p && isspace(*p)) ++p;
	if (p > s) { memmove(s, p, p_end - s + 1); p_end -= p - s; }
	for (p = p_end; p > s && isspace(p[-1]); --p);
	*p = 0;
}

static void LoadLauncherConfig(void)
{
	char conf_filename[MAX_PATH];
	char line[1024], *eq = 0;
	int n = 0;

	cry_sprintf(conf_filename, "%s/%s", fopenwrapper_basedir, LINUX_LAUNCHER_CONF);
	FILE *fp = fopen(conf_filename, "r");
	if (!fp) return;
	while (true)
	{
		++n;
		if (!fgets(line, sizeof line - 1, fp)) break;
		line[sizeof line - 1] = 0;
		strip(line);
		if (!line[0] || line[0] == '#') continue;
		eq = strchr(line, '=');
		if (!eq)
		{
			fprintf(stderr, "'%s': syntax error in line %i\n",
					conf_filename, n);
			exit(EXIT_FAILURE);
		}
		*eq = 0;
		strip(line);
		strip(++eq);

		if (!strcasecmp(line, "autoload"))
		{
			if (strlen(eq) >= linux_autoload_level_maxsize)
			{
				fprintf(stderr, "'%s', line %i: autoload value too long\n",
						conf_filename, n);
				exit(EXIT_FAILURE);
			}
			strcpy(linux_autoload_level, eq);
		} else
		{
			fprintf(stderr, "'%s': unrecognized config variable '%s' in line %i\n",
					conf_filename, line, n);
			exit(EXIT_FAILURE);
		}
	}
	fclose(fp);
}

static struct android_state
{
	bool m_isAppReady;
	bool m_isInitialized;
	struct android_app* m_state;

	android_state() : 
		m_isAppReady(false),
		m_isInitialized(false),
		m_state(NULL)
	{
	}
} g_android_state;


int RunGame(const char *) __attribute__ ((noreturn));

DLL_EXPORT const char* androidGetInternalPath()
{
#if defined(DEDICATED_SERVER)
	ANativeActivity* activity = g_android_state.m_state->activity;
	return activity->internalDataPath;
#else
	return SDL_AndroidGetInternalStoragePath();
#endif
}

const char* g_androidPakPath = "";

DLL_EXPORT const char* androidGetPakPath()
{
	return g_androidPakPath;
}

DLL_EXPORT const char* androidGetPackageName()
{
	return SDLExt_AndroidGetPackageName();
}

DLL_EXPORT const char* androidGetExpFilePath()
{
	static char path[1024];

	if (path[0] == '\0')
	{
		cry_strcpy(path, androidGetPakPath());
		cry_strcat(path, "/Android/obb/");
		cry_strcat(path, androidGetPackageName());
		cry_strcat(path, "/");
	}
	return path;
}

char g_androidMainExpName[256] = {0};
char g_androidPatchExpName[256] = {0};
    
DLL_EXPORT const char* androidGetMainExpName()
{
	return g_androidMainExpName; 
}

DLL_EXPORT const char* androidGetPatchExpName()
{
	return g_androidPatchExpName;
}

DLL_EXPORT const char* androidGetAssetFileName()
{
	return "assets.ogg";
}

DLL_EXPORT AAssetUpr* androidGetAssetUpr()
{
	return SDLExt_GetAssetUpr();
}

#if !defined(_RELEASE)
struct COutputPrintSink : public IOutputPrintSink
{
	virtual void Print( const char *inszText )
	{
		LOGI("%s", inszText);
	}
};

COutputPrintSink g_androidPrintSink;
#endif

#if defined(ANDROID_OBB)

/// Return whether the given filename conforms with APK expansion file naming
/// convension.
///
/// \param isMain If filename is expansion file, isMain indicate if it is main
///               expansion file.
/// \param filename The filename to be checked.
///
bool IsExpansionFileName(bool &isMain, const char *filename)
{
	bool possibleMain = false;

	if (filename)
	{
		const char *src = filename;
		const char *dst = strchr(filename, '.');
		int length = 0;

		/// Check whether filename starts with "main." or "patch."
		if (dst)
		{
			length = dst - src + 1;
			if (strncmp("main.", src, length) == 0)
			{
				possibleMain = true;
			}
			else if (!strncmp("patch.", src, length) == 0)
			{
				return false;
			}
		}
		else {
			return false;
		}

		/// update src and dst.
		src = dst + 1;
		if (src)
		{
			dst = strchr(src, '.');
		}
		else {
			return false;
		}

		/// Check whether expansion version number is composed of digits only.
		if (dst)
		{
			while (src != dst) {
				if (!isdigit(*src)) {
					return false;
				}
				++src;
			}
		}
		else {
			return false;
		}

		/// Check whether if the package name matches.
		++src;
		const char *package = androidGetPackageName();
		length = strlen(package);

		if (src && strncmp(src, package, length) != 0)
		{
			return false;
		}

		src += length;
		if (src && *src == '.')
		{
			++src;
		}
		else
		{
			return false;
		}

		/// Check if the filename ends with "obb".
		if (strcmp(src, "obb") != 0)
		{
			return false;
		}

		/// All the above tests are passed, the file is an expansion file.
		if (possibleMain)
		{
			isMain = true;
		}
		return true;
	}
	return false;
}

/// Search main and patch expansion file in the specified directory, if Main
/// expansion file exist, update g_androidMainExpName (and g_androidPatchExpName
/// if patch expansion file exists), and returns true. Otherwise return false. 
///
bool SearchExpansionFiles(const char *path)
{
	struct dirent *pDirent = NULL;
	DIR *pDir = NULL;
	bool result = false;

	if (path)
	{
		pDir = opendir(path);
		if (pDir == NULL)
		{
			return false;
		}

		while ((pDirent = readdir(pDir)) != NULL)
		{
			// If it is a regular file, check the 
			if (pDirent->d_type == DT_REG)
			{
				bool isMain = false;
				const char *filename = pDirent->d_name;
				if (IsExpansionFileName(isMain, filename))
				{
					if (isMain)
					{
						cry_strcpy(g_androidMainExpName, filename);
						result = true;
					}
					else
					{
						cry_strcpy(g_androidPatchExpName, filename);
					}
				}
			}
		}
		closedir(pDir);
	}
	// Clear g_androidPatchExpName if no Main expansion file is found, since
	// patch expansion file cannot be used on its own.
	if (result == false)
	{
		g_androidPatchExpName[0] = 0;
	}

	return result;
}
#endif

int RunGame(const char *commandLine)
{
	char absPath[ MAX_PATH];
	memset(absPath,0,sizeof(char)* MAX_PATH);

	if (!getcwd(absPath,sizeof(char)* MAX_PATH))
		RunGame_EXIT(1);
	LOGI( "CWD = %s", absPath );

#if !defined(DEDICATED_SERVER)
	LOGI( "InternalStoragePath = %s", SDL_AndroidGetInternalStoragePath()); 
	LOGI( "ExternalStoragePath = %s", SDL_AndroidGetExternalStoragePath());
	LOGI( "ExternalStorageDirectory = %s", SDLExt_AndroidGetExternalStorageDirectory());
#endif

	// Try to figure out where the PAK files are stored
	const char* paths[] = {
		androidGetInternalPath(),
		"/sdcard",
		"/storage/sdcard",
		"/storage/sdcard0",
		"/storage/sdcard1",
		"/storage/usbdrive",
		SDL_AndroidGetExternalStoragePath(),
		SDLExt_AndroidGetExternalStorageDirectory(),
	};
	for (int i = 0; i < DRX_ARRAY_COUNT(paths); ++i )
	{
		char path[1024];
		cry_strcpy(path, paths[i]);
#if !defined(ANDROID_OBB)
		cry_strcat(path, "/GameSDK/GameData.pak");
		LOGI( "Searching for %s", path);
		FILE* f = fopen( path, "r" );
		if (f != NULL)
		{
			g_androidPakPath = paths[i];
			fclose(f);
			break;
		}
#else
		cry_strcat(path, "/Android/obb/");
		cry_strcat(path, androidGetPackageName());
		cry_strcat(path, "/");
		LOGI( "Searching expansion files in %s", path);
		if (SearchExpansionFiles(path))
		{
			g_androidPakPath = paths[i];
			break;
		}
#endif
	}

#if defined(ANDROID_OBB)
	// Try to search assets within apk package.
	AAssetUpr *mgr = androidGetAssetUpr();
	if (mgr)
	{
		AAsset *asset = AAssetUpr_open(mgr, androidGetAssetFileName(), AASSET_MODE_STREAMING);
		if (asset && strlen(g_androidPakPath) == 0)
		{
			g_androidPakPath = ".";
		}
		if (asset)
		{
			AAsset_close(asset);
		}
	}
#endif
	if (strlen(g_androidPakPath) == 0)
	{
		LOGE( "Unable to locate system.cfg files.  Exiting!" );
		RunGame_EXIT(1);
	}
	LOGI( "system.cfg found in: %s", g_androidPakPath );

	int exitCode = 0;

	size_t uDefStackSize;

	if (!IncreaseResourceMaxLimit(RLIMIT_CORE, RLIM_INFINITY) || !GetDefaultThreadStackSize(&uDefStackSize) ||
		!IncreaseResourceMaxLimit(RLIMIT_STACK, RLIM_INFINITY*uDefStackSize))
		RunGame_EXIT(1);

	SSystemInitParams startupParams;
	memset(&startupParams, 0, sizeof(SSystemInitParams));

	startupParams.hInstance = 0;
	cry_strcpy(startupParams.szSystemCmdLine, commandLine);
#if defined(DEDICATED_SERVER)
	startupParams.sLogFileName = "Server.log";
	startupParams.bDedicatedServer = true;
#else
	startupParams.sLogFileName = "Game.log";
#endif
	startupParams.pUserCallback = NULL;
#if !defined(_RELEASE)
	startupParams.pPrintSync = &g_androidPrintSink;
#endif

#if !defined(_LIB)
	SetModulePath((strcmp(absPath,"/") == 0) ? "" : absPath);
	HMODULE systemlib = CryLoadLibraryDefName("CrySystem");
	if(!systemlib)
	{
		LOGE("Failed to load CrySystem: %s", dlerror());
		exit(1);
	}
#endif

	chdir( androidGetPakPath() );

	HMODULE gameDll = 0;

#ifndef _LIB
	CEngineConfig engineCfg;
	// workaround: compute .so name from dll name
	string dll_name = engineCfg.m_gameDLL.c_str();

	string::size_type extension_pos = dll_name.rfind(".dll");
	string shared_lib_name = string(CrySharedLibraryPrefix) + dll_name.substr(0, extension_pos) + string(CrySharedLibraryExtension);

	gameDll = CryLoadLibrary(shared_lib_name.c_str());
	if( !gameDll )
	{
		LOGE("ERROR: failed to load GAME DLL (%s)\n", dlerror());
		RunGame_EXIT(1);
	}
	// get address of startup function
	IGameStartup::TEntryFunction CreateGameStartup = (IGameStartup::TEntryFunction)CryGetProcAddress(gameDll, "CreateGameStartup");
	if (!CreateGameStartup)
	{
		// dll is not a compatible game dll
		CryFreeLibrary(gameDll);
		LOGE("ERROR: Specified Game DLL is not valid!\n");
		RunGame_EXIT(1);
	}
#endif //_LIB

	// create the startup interface
	IGameStartup* pGameStartup = CreateGameStartup();

	const char *const szAutostartLevel
		= linux_autoload_level[0] ? linux_autoload_level : NULL;

	if (!pGameStartup)
	{
		LOGE("ERROR: Failed to create the GameStartup Interface!\n");
		RunGame_EXIT(1);
	}

	// run the game
	IGame *game = pGameStartup->Init(startupParams);
	if (game)
	{
		exitCode = pGameStartup->Run(szAutostartLevel);
		pGameStartup->Shutdown();
		pGameStartup = 0;
		RunGame_EXIT(exitCode);
	}

	// if initialization failed, we still need to call shutdown
	pGameStartup->Shutdown();
	pGameStartup = 0;

	fprintf(stderr, "ERROR: Failed to initialize the GameStartup Interface!\n");
	RunGame_EXIT(exitCode);
}

// An unreferenced function.  This function is needed to make sure that
// unneeded functions don't make it into the final executable.  The function
// section for this function should be removed in the final linking.
void this_function_is_not_used(void)
{
}

//-------------------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-------------------------------------------------------------------------------------
int _main(int argc, char **argv)
{
	int err;

	LoadLauncherConfig();

#if !defined(DEDICATED_SERVER)
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER ) < 0)
	{
		fprintf(stderr, "ERROR: SDL initialization failed: %s\n",
			SDL_GetError());
		exit(EXIT_FAILURE);
	}
	atexit(SDL_Quit);
#endif

	// Build the command line.
	// We'll attempt to re-create the argument quoting that was used in the
	// command invocation.
	size_t cmdLength = 0;
	char needQuote[argc];
	for (int i = 0; i < argc; ++i)
	{
		bool haveSingleQuote = false, haveDoubleQuote = false, haveBrackets = false;
		bool haveSpace = false;
		for (const char *p = argv[i]; *p; ++p)
		{
			switch (*p)
			{
			case '"': haveDoubleQuote = true; break;
			case '\'': haveSingleQuote = true; break;
			case '[': case ']': haveBrackets = true; break;
			case ' ': haveSpace = true; break;
			default: break;
			}
		}
		needQuote[i] = 0;
		if (haveSpace || haveSingleQuote || haveDoubleQuote || haveBrackets)
		{
			if (!haveSingleQuote)
				needQuote[i] = '\'';
			else if (!haveDoubleQuote)
				needQuote[i] = '"';
			else if (!haveBrackets)
				needQuote[i] = '[';
			else
			{
				fprintf(stderr, "DRXSIS LinuxLauncher Error: Garbled command line\n");
				exit(EXIT_FAILURE);
			}
		}
		cmdLength += strlen(argv[i]) + (needQuote[i] ? 2 : 0);
		if (i > 0)
			++cmdLength;
	}
	char cmdLine[cmdLength + 1], *q = cmdLine;
	for (int i = 0; i < argc; ++i)
	{
		if (i > 0)
			*q++ = ' ';
		if (needQuote[i])
			*q++ = needQuote[i];
		strcpy(q, argv[i]);
		q += strlen(q);
		if (needQuote[i])
			if (needQuote[i] == '[')
				*q++ = ']';
			else
				*q++ = needQuote[i];
	}
	*q = 0;
	assert(q - cmdLine == cmdLength);

#if CAPTURE_REPLAY_LOG
	// Since Android doesn't support native command line argument, please
	// uncomment the following line if -memreplay is needed.
	// CryGetIMemReplay()->StartOnCommandLine("-memreplay");
	CryGetIMemReplay()->StartOnCommandLine(cmdLine);
#endif
	return RunGame(cmdLine);
}

static void handle_cmd(struct android_app* app, int32_t cmd)
{
	/**
	* On intial startup:
	* APP_CMD_START
	* APP_CMD_RESUME
	* APP_CMD_INIT_WINDOW
	* APP_CMD_GAINED_FOCUS
	*
	* On go to home:
	* APP_CMD_PAUSE
	* APP_CMD_LOST_FOCUS
	* APP_CMD_TERM_WINDOW
	* APP_CMD_SAVE_STATE
	* APP_CMD_STOP
	* 
	* On return to app:
	* APP_CMD_START
	* APP_CMD_RESUME
	* APP_CMD_INIT_WINDOW
	* APP_CMD_GAINED_FOCUS
	*/
	switch(cmd)
	{
	case APP_CMD_INIT_WINDOW:
		LOGI("Init window");
		g_android_state.m_isAppReady = true;
		break;
	case APP_CMD_TERM_WINDOW:
		LOGI("Terminiate window");
		break;
	case APP_CMD_GAINED_FOCUS:
		LOGI("Gained focus");
		break;
	case APP_CMD_LOST_FOCUS:
		LOGI("Lost focus");
		break;
	case APP_CMD_LOW_MEMORY:
		LOGI("Low memory");
		break;
	case APP_CMD_START:
		LOGI("Started");
		break;
	case APP_CMD_RESUME:
		LOGI("Resumed");
		break;
	case APP_CMD_SAVE_STATE:
		LOGI("Save app state");
		break;
	case APP_CMD_PAUSE:
		LOGI("Paused");
		break;
	case APP_CMD_STOP:
		LOGI("Stopped");
		break;
	default:
		break;
	}
}

static int32_t handle_input(struct android_app* app, AInputEvent * event)
{
	if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		LOGI("Motion Event (%f,%f)",AMotionEvent_getX(event,0), AMotionEvent_getY(event,0));
		return 1;
	}
	return 0;
}

/**
 * Entry point when running as dedicated server.
 */
void android_main(struct android_app* state)
{
	LOGI("Android Main");
	app_dummy();
	g_android_state.m_state = state;
	state->onAppCmd = handle_cmd;
	state->onInputEvent = handle_input;
	LOGI("Loop cycle begin");
	while(1)
	{
		int ident;
		int fdesc;
		int events;
		struct android_poll_source* source;

		while((ident = ALooper_pollAll(0, &fdesc, &events, (void**)&source)) >= 0)
		{
			LOGI("Loop event");
			// process this event
			if (source)
				source->process(state, source);
		}

		if( g_android_state.m_isAppReady )
		{
			if( ! g_android_state.m_isInitialized )
			{
				g_android_state.m_isInitialized = true;
				_main(0, NULL);
			}
		}
	}
	LOGI("Loop cycle end");
}

/**
 * Entry point when running in SDL framework.
 */
extern "C" int SDL_main(int argc, char* argv[])
{
	_main(0, NULL);
	return 0;
}

// vim:sw=4:ts=4:si:noet

