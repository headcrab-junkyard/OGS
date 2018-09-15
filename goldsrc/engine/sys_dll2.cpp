/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief Listen and Dedicated Server API implementation

#include "quakedef.h"
#include "engine_launcher_api.h"
#include "engine_hlds_api.h"
#include "igameuifuncs.h"
#include "iengine.h"
#include "interface.h"

qboolean gbDedicatedServer{ false };

char *gsPostRestartCmdLineArgs{ nullptr };

void Sys_InitGame(const char *lpOrgCmdLine, const char *pBaseDir /*TODO: szBaseDir?*/, void *pwnd, int bIsDedicated)
{
	static quakeparms_t parms{}; // TODO: (non-)static?

	memset(&parms, 0, sizeof(parms));

#ifdef __linux__
	//signal(SIGFPE, floating_point_exception_handler);
	signal(SIGFPE, SIG_IGN);
#endif

#if defined(GLQUAKE) or defined(sun) or defined(SWDS)
	parms.memsize = 16*1024*1024; // TODO: 16Mb
#else
	parms.memsize = 8*1024*1024; // TODO: 8Mb; 5861376 in QW
#endif

	/*
	if ((t = COM_CheckParm ("-heapsize")) != 0 && t + 1 < com_argc)
		parms.memsize = Q_atoi (com_argv[t + 1]) * 1024;

	if ((t = COM_CheckParm ("-mem")) != 0 && t + 1 < com_argc)
		parms.memsize = Q_atoi (com_argv[t + 1]) * 1024 * 1024;
	*/

	parms.membase = malloc(parms.memsize);
	
	//if (!parms.membase)
		//Sys_Error("Insufficient memory.\n");
	
	parms.basedir = (char*)pBaseDir; // TODO: "."
	
	/*
	static char cwd[1024];
	_getcwd(cwd, sizeof(cwd));
	if(cwd[Q_strlen(cwd) - 1] == '\\')
		cwd[Q_strlen(cwd) - 1] = 0;
	parms.basedir = cwd;
	*/
	
	parms.cachedir = NULL; // TODO
	
	// TODO: cmdline string parsing/tokenization
	//COM_InitArgv(argc, argv); // TODO: parms.argc, parms.argv

	/*
	// dedicated server ONLY!
	if(!COM_CheckParm("-dedicated"))
	{
		memcpy(newargv, argv, argc * 4);
		newargv[argc] = "-dedicated";
		argc++;
		argv = newargv;
		COM_InitArgv(argc, argv);
	}
	*/
	
	//parms.argc = com_argc; // argc
	//parms.argv = com_argv; // argv
	
	isDedicated = bIsDedicated; // TODO: was (COM_CheckParm ("-dedicated") != 0);
	
	if(isDedicated)
		cls.state = ca_dedicated;
	
	//printf("Host_Init\n");
	Host_Init(&parms);
	
	//oldtime = Sys_FloatTime();
	
	Host_InitializeGameDLL();
};

int RunListenServer(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	// TODO: Whole bunch of Sys_Init* calls

	FileSystem_Init(basedir, (void*)filesystemFactory);
	
	// TODO: CWindowManager::CreateGameWindow(); // IGame::CreateGameWindow

	if(!gpEngine->Load(false, basedir, cmdline))
		return EXIT_FAILURE;

	while(true)
		gpEngine->Frame();

	gpEngine->Unload();

	// TODO: CWindowManager::DestroyGameWindow(); // IGame::DestroyGameWindow

	FileSystem_Shutdown();
	
	// TODO: Whole bunch of Sys_Shutdown* calls

	return EXIT_SUCCESS;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CEngineAPI final : public IEngineAPI
{
public:
	CEngineAPI();
	~CEngineAPI();

	int Run(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) override;
};

EXPOSE_SINGLE_INTERFACE(CEngineAPI, IEngineAPI, VENGINE_LAUNCHER_API_VERSION);

CEngineAPI::CEngineAPI() = default;
CEngineAPI::~CEngineAPI() = default;

int CEngineAPI::Run(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	return RunListenServer(instance, basedir, cmdline, postRestartCmdLineArgs, launcherFactory, filesystemFactory);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO
/*
class CGameUIFuncs final : public IGameUIFuncs
{
public:
	CGameUIFuncs();
	~CGameUIFuncs();

	bool IsKeyDown(char const *keyname, bool &isdown) override;

	const char *Key_NameForKey(int keynum) override;
	const char *Key_BindingForKey(int keynum) override;

	vgui2::KeyCode GetVGUI2KeyCodeForBind(const char *bind) override;

	void GetVideoModes(struct vmode_s **liststart, int *count) override;
	void GetCurrentVideoMode(int *wide, int *tall, int *bpp) override;

	void GetCurrentRenderer(char *name, int namelen,
	                        int *windowed, int *hdmodels,
	                        int *addons_folder, int *vid_level) override;
	//void GetCurrentRenderer(char *name, int namelen, int *windowed) override;

	bool IsConnectedToVACSecureServer() override;

	int Key_KeyStringToKeyNum(const char *string) override;
};

EXPOSE_SINGLE_INTERFACE(CGameUIFuncs, IGameUIFuncs, GAMEUIFUNCS_INTERFACE_VERSION);

CGameUIFuncs::CGameUIFuncs() = default;
CGameUIFuncs::~CGameUIFuncs() = default;

bool CGameUIFuncs::IsKeyDown(char const *keyname, bool &isdown)
{
	return false; // TODO: Key_IsDown(keyname, isdown);
};

const char *CGameUIFuncs::Key_NameForKey(int keynum)
{
	return Key_NameForKey(keynum);
};

const char *CGameUIFuncs::Key_BindingForKey(int keynum)
{
	return Key_BindingForKey(keynum);
};

vgui2::KeyCode CGameUIFuncs::GetVGUI2KeyCodeForBind(const char *bind)
{
	return vgui2::KeyCode::KEY_NONE;
};

void CGameUIFuncs::GetVideoModes(struct vmode_s **liststart, int *count){};

void CGameUIFuncs::GetCurrentVideoMode(int *wide, int *tall, int *bpp){};

void CGameUIFuncs::GetCurrentRenderer(char *name, int namelen, int *windowed, int *hdmodels, int *addons_folder, int *vid_level){};

bool CGameUIFuncs::IsConnectedToVACSecureServer()
{
	return false;
};

int CGameUIFuncs::Key_KeyStringToKeyNum(const char *string)
{
	return Key_KeyStringToKeyNum(string);
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDedicatedServerAPI final : public IDedicatedServerAPI
{
public:
	CDedicatedServerAPI();
	~CDedicatedServerAPI();

	bool Init(const char *basedir, const char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) override;
	int Shutdown() override;

	bool RunFrame() override;

	void AddConsoleText(const char *text) override;

	void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, /*const*/ char *pszMap) override;
};

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

bool CDedicatedServerAPI::Init(const char *basedir, const char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	FileSystem_Init(basedir, (void*)filesystemFactory);
	
	if(!gpEngine->Load(true, basedir, cmdline))
		return false;
	
	FileSystem_Shutdown();

	return true;
};

int CDedicatedServerAPI::Shutdown()
{
	gpEngine->Unload();

	return 0;
};

bool CDedicatedServerAPI::RunFrame()
{
	gpEngine->Frame();
	return true; // TODO
};

void CDedicatedServerAPI::AddConsoleText(const char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, /*const*/ char *pszMap)
{
	Host_UpdateStatus(fps, nActive, nMaxPlayers, pszMap);
};