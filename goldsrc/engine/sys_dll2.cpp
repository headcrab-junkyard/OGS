/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2020-2024 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief Listen and Dedicated Server API implementation

#include "quakedef.h"
#include "engine_launcher_api.h"
#include "engine_hlds_api.h"
#include "igameuifuncs.h"
#include "dedicated/idedicatedexports.h"
#include "iengine.h"
#include "igame.h"

#include "traceinit.h"

qboolean gbDedicatedServer{false};

char *gsPostRestartCmdLineArgs{nullptr};

IDedicatedExports *gpDedicatedExports{nullptr};

/*extern "C"*/ void DedicatedExports_Print(const char *text)
{
	if(gpDedicatedExports)
		gpDedicatedExports->Sys_Printf(text);
};

/*
================
Sys_InitMemory
================
*/
void Sys_InitMemory()
{
	host_parms.memsize = 128 * 1024 * 1024; // TODO: 128Mb; should the dedicated server use the same amount?
	
	int t;
	
	if((t = COM_CheckParm("-heapsize")) != 0 && t + 1 < com_argc)
		host_parms.memsize = Q_atoi(com_argv[t + 1]) * 1024;
	
	// TODO: -minmemory
	
	host_parms.membase = malloc(host_parms.memsize);
	
	if(!host_parms.membase)
		Sys_Error("Insufficient memory.\n");
};

/*
================
Sys_ShutdownMemory
================
*/
void Sys_ShutdownMemory()
{
	// TODO
};

/*
================
Sys_InitLauncherInterface
================
*/
void Sys_InitLauncherInterface()
{
	// TODO
};

/*
================
Sys_ShutdownLauncherInterface
================
*/
void Sys_ShutdownLauncherInterface()
{
	// TODO
};

/*
================
Sys_InitAuthentication
================
*/
void Sys_InitAuthentication()
{
	// TODO: STEAM Auth Server
};

/*
================
Sys_ShutdownAuthentication
================
*/
void Sys_ShutdownAuthentication()
{
	// TODO
};

void Sys_InitGame(const char *lpOrgCmdLine, const char *pBaseDir /*TODO: szBaseDir?*/, void *pwnd, int bIsDedicated)
{
	// TODO: pwnd should be handled
	// TODO: &mainwindow = pwnd;?
	
	memset(&host_parms, 0, sizeof(host_parms));

#ifdef __linux__
	//signal(SIGFPE, floating_point_exception_handler); // TODO
	signal(SIGFPE, SIG_IGN);
#endif
	
	isDedicated = bIsDedicated;
	
	if(isDedicated)
		cls.state = ca_dedicated;
	
	TraceInit("Sys_Shutdown()", "Sys_Init()");
	Sys_Init();
	
	// TODO: "Launcher" static string here
	
	TraceInit("Sys_ShutdownMemory()", "Sys_InitMemory()");
	Sys_InitMemory();
	
	TraceInit("Sys_ShutdownLauncherInterface()", "Sys_InitLauncherInterface()");
	Sys_InitLauncherInterface();
	
	host_parms.basedir = (char*)pBaseDir;
	
	/*
	static char cwd[1024];
	_getcwd(cwd, sizeof(cwd));
	if(cwd[Q_strlen(cwd) - 1] == '\\')
		cwd[Q_strlen(cwd) - 1] = 0;
	host_parms.basedir = cwd;
	*/
	
	host_parms.cachedir = NULL; // TODO
	
	
	
	TraceInit("Host_Shutdown()", "Host_Init( &host_parms )");
	Host_Init(&host_parms);
	
	TraceInit("Sys_ShutdownAuthentication()", "Sys_InitAuthentication()");
	Sys_InitAuthentication();
	
	//oldtime = Sys_FloatTime(); // TODO
	
		Host_InitializeGameDLL();
		NET_Config(true);
};

void Sys_ShutdownGame()
{
	TraceShutdown("Host_Shutdown()");
	Host_Shutdown();
	
	TraceShutdown("Sys_ShutdownLauncherInterface()");
	Sys_ShutdownLauncherInterface();
	
	TraceShutdown("Sys_ShutdownAuthentication()");
	Sys_ShutdownAuthentication();
	
	TraceShutdown("Sys_ShutdownMemory()");
	Sys_ShutdownMemory();
	
	TraceShutdown("Sys_Shutdown()");
	Sys_Shutdown();
};

int RunListenServer(void *instance, const char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	// TODO: make sized
	const char *OrigCmd = cmdline;
	
	TraceInit("Sys_ShutdownArgv()", "Sys_InitArgv( OrigCmd )");
	Sys_InitArgv((char*)lpOrgCmdLine); // TODO: was COM_InitArgv(c, v);
	
	//host_parms.argc = com_argc; // TODO: already done in Sys_InitArgv
	//host_parms.argv = com_argv; // TODO: already done in Sys_InitArgv
	
	TraceInit("FileSystem_Shutdown()", "FileSystem_Init(basedir, (void *)filesystemFactory)");
	FileSystem_Init(basedir, (void*)filesystemFactory);
	
	// TODO: exec something? "exec %s"
	
	gpGame->CreateGameWindow();

	if(!gpEngine->Load(false, basedir, cmdline))
		return EXIT_FAILURE;

	while(true)
		gpEngine->Frame();

	gpEngine->Unload();

	// TODO: IGame::DestroyGameWindow()?
	
	// TODO: CrashInitializingVideoMode  CrashInitializingVideoMode
	
	TraceShutdown("FileSystem_Shutdown()");
	FileSystem_Shutdown();
	
	TraceShutdown("Sys_ShutdownArgv()");
	Sys_ShutdownArgv();
	
	// TODO: Whole bunch of Sys_Shutdown* calls?

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

bool CGameUIFuncs::IsKeyDown(const char *keyname, bool &isdown)
{
	return false; // TODO: Key_IsDown(keyname, isdown);
};

const char *CGameUIFuncs::Key_NameForKey(int keynum)
{
	return Key_KeynumToString(keynum);
};

const char *CGameUIFuncs::Key_BindingForKey(int keynum)
{
	return Key_GetBinding(keynum);
};

vgui2::KeyCode CGameUIFuncs::GetVGUI2KeyCodeForBind(const char *bind)
{
	return vgui2::KeyCode::KEY_NONE;
};

void CGameUIFuncs::GetVideoModes(struct vmode_s **liststart, int *count)
{
	// TODO
};

void CGameUIFuncs::GetCurrentVideoMode(int *wide, int *tall, int *bpp)
{
	if(wide)
		*wide = vid.width;
	
	if(tall)
		*tall = vid.height;
	
	if(bpp)
		*bpp = 0; // TODO
};

void CGameUIFuncs::GetCurrentRenderer(char *name, int namelen, int *windowed, int *hdmodels, int *addons_folder, int *vid_level)
{
	// TODO
};

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

	void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, /*const*/ char *pszMap) override;
private:
	const char *m_OrigCmd{nullptr}; // TODO: make sized?
};

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

bool CDedicatedServerAPI::Init(const char *basedir, const char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	gpDedicatedExports = (IDedicatedExports*)launcherFactory(VENGINE_DEDICATEDEXPORTS_API_VERSION, nullptr);
	
	if(!gpDedicatedExports)
		return false;
	
	// TODO: check for "-nobreakpad"
	
	
	m_OrigCmd = cmdline;
	
	TraceInit("Sys_ShutdownArgv()", "Sys_InitArgv( m_OrigCmd )");
	Sys_InitArgv(m_OrigCmd);
	
	TraceInit("FileSystem_Shutdown()", "FileSystem_Init(basedir, (void*)filesystemFactory");
	FileSystem_Init(basedir, reinterpret_cast<void*>(filesystemFactory));
	
	if(!gpEngine->Load(true, basedir, cmdline))
		return false;
	
	// TODO: "exec %s" here (or during shutdown), mb some server config
	
	return true;
};

int CDedicatedServerAPI::Shutdown()
{
	gpEngine->Unload();
	
	TraceShutdown("FileSystem_Shutdown()");
	FileSystem_Shutdown();
	
	TraceShutdown("Sys_ShutdownArgv()");
	Sys_ShutdownArgv();
	
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