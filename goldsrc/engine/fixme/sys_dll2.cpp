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

void Sys_InitGame(char *lpOrgCmdLine, char *pBaseDir /*TODO: szBaseDir?*/, void *pwnd, int bIsDedicated)
{
	static quakeparms_t parms{}; // TODO: (non-)static?

	memset(&parms, 0, sizeof(parms));

#ifdef __linux__
	//signal(SIGFPE, floating_point_exception_handler);
	signal(SIGFPE, SIG_IGN);
#endif

#if defined(GLQUAKE) or defined(sun)
	parms.memsize = 16*1024*1024;
#else
	parms.memsize = 8*1024*1024; // TODO: 5861376 in QW
#endif

	parms.membase = malloc(parms.memsize);
	parms.basedir = pBaseDir; // TODO: "."
	
	parms.cachedir = NULL; // TODO
	
	// TODO: cmdline string parsing/tokenization
	//COM_InitArgv(argc, argv); // TODO: parms.argc, parms.argv

	//parms.argc = com_argc;
	//parms.argv = com_argv;
	
	isDedicated = bIsDedicated; // TODO: was (COM_CheckParm ("-dedicated") != 0);
	
	//printf("Host_Init\n");
	Host_Init(&parms);
};

int RunListenServer(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	// TODO: Whole bunch of Sys_Init* calls

	// TODO: CWindowManager::CreateGameWindow(); // IGame::CreateGameWindow

	if(!gpEngine->Load(false, basedir, cmdline))
		return EXIT_FAILURE;

	while(true)
		gpEngine->Frame();

	gpEngine->Unload();

	// TODO: CWindowManager::DestroyGameWindow(); // IGame::DestroyGameWindow

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

	void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, const char *pszMap) override;
};

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

bool CDedicatedServerAPI::Init(const char *basedir, const char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	if(!gpEngine->Load(true, basedir, cmdline))
		return false;

	return true;
};

int CDedicatedServerAPI::Shutdown()
{
	gpEngine->Unload();

	return 0;
};

bool CDedicatedServerAPI::RunFrame()
{
	//gpEngine->Frame();
	return false;
};

void CDedicatedServerAPI::AddConsoleText(const char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, const char *pszMap)
{
	Host_UpdateStatus(fps, nActive, nMaxPlayers, pszMap);
};