/// @file
/// @brief Listen and Dedicated Server API implementation

#include "quakedef.h"
#include "engine_launcher_api.h"
#include "engine_hlds_api.h"
#include "igameuifuncs.h"
#include "iengine.h"
#include "interface.h"

int RunListenServer(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
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

class CEngineAPI final : public IEngineAPI
{
public:
	CEngineAPI();
	~CEngineAPI();
	
	int Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) override;
};

EXPOSE_SINGLE_INTERFACE(CEngineAPI, IEngineAPI, VENGINE_LAUNCHER_API_VERSION);

int CEngineAPI::Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
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
	return Key_IsDown(keyname, isdown);
};

const char *CGameUIFuncs::Key_NameForKey(int keynum)
{
	return Key_NameForKey(keynum);
};

const char *CGameUIFuncs::Key_BindingForKey(int keynum)
{
	return Key_BindingForKey(keynum);
};

vgui::KeyCode CGameUIFuncs::GetVGUI2KeyCodeForBind(const char *bind)
{
	return 0;
};

void CGameUIFuncs::GetVideoModes(struct vmode_s **liststart, int *count)
{
};

void CGameUIFuncs::GetCurrentVideoMode(int *wide, int *tall, int *bpp)
{
};

void CGameUIFuncs::GetCurrentRenderer(char *name, int namelen, int *windowed, int *hdmodels, int *addons_folder, int *vid_level)
{
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
	
	bool Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) override;
	int Shutdown() override;

	bool RunFrame() override;

	void AddConsoleText(char *text) override;

	void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap) override;
};

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, DEDICATEDSERVERAPI_INTERFACE_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

bool CDedicatedServerAPI::Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
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

void CDedicatedServerAPI::AddConsoleText(char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
};