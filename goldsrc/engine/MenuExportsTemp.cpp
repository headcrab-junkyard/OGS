/// @file

#include "quakedef.h"
#include "MenuExportsTemp.hpp"

//#ifdef _WIN32
//#include "winquake.h"
//#endif

typedef enum { MS_WINDOWED,
	           MS_FULLSCREEN,
	           MS_FULLDIB,
	           MS_UNINIT } modestate_t;

extern "C" modestate_t modestate;
extern "C" int vid_modenum;
extern "C" int VID_NumModes();
extern "C" const char *VID_GetModeDescription(int mode);
extern "C" vmode_t *VID_GetModePtr(int mode);

EXPOSE_SINGLE_INTERFACE(CMenuExportsTemp, IMenuExportsTemp, OGS_MENUEXPORTS_INTERFACE_VERSION);

CMenuExportsTemp::CMenuExportsTemp() = default;
CMenuExportsTemp::~CMenuExportsTemp() = default;

void CMenuExportsTemp::Cbuf_AddText(const char *text)
{
	::Cbuf_AddText(text);
};

void CMenuExportsTemp::Cbuf_InsertText(const char *text)
{
	::Cbuf_InsertText(text);
};

void CMenuExportsTemp::Cmd_AddCommand(const char *name, xcommand_t function)
{
	::Cmd_AddCommand((char*)name, function);
};

void CMenuExportsTemp::Cvar_Set(const char *name, const char *value)
{
	::Cvar_Set(name, value);
};

void CMenuExportsTemp::Cvar_SetValue(const char *name, float value)
{
	::Cvar_SetValue(name, value);
};

const char *CMenuExportsTemp::Cvar_VariableString(const char *name) const
{
	return ::Cvar_VariableString(name);
};

float CMenuExportsTemp::Cvar_VariableValue(const char *name) const
{
	return ::Cvar_VariableValue(name);
};

void CMenuExportsTemp::S_LocalSound(const char *sample)
{
	::S_LocalSound(sample);
};

void CMenuExportsTemp::S_ExtraUpdate()
{
	::S_ExtraUpdate();
};

void CMenuExportsTemp::Draw_FadeScreen()
{
	::Draw_FadeScreen();
};

void CMenuExportsTemp::Draw_ConsoleBackground(int lines)
{
	::Draw_ConsoleBackground(lines);
};

qpic_t *CMenuExportsTemp::Draw_CachePic(const char *name)
{
	return ::Draw_CachePic(name);
};

void CMenuExportsTemp::Draw_Character(int x, int y, int num)
{
	::Draw_Character(x, y, num);
};

void CMenuExportsTemp::Draw_Pic(int x, int y, qpic_t *pic)
{
	::Draw_Pic(x, y, pic);
};

void CMenuExportsTemp::Draw_TransPic(int x, int y, qpic_t *pic)
{
	::Draw_TransPic(x, y, pic);
};

void CMenuExportsTemp::Draw_TransPicTranslate(int x, int y, qpic_t *pic, byte *translation)
{
	::Draw_TransPicTranslate(x, y, pic, translation);
};

void CMenuExportsTemp::VID_LockBuffer()
{
	//::VID_LockBuffer(); // TODO
};

void CMenuExportsTemp::VID_UnlockBuffer()
{
	//::VID_UnlockBuffer(); // TODO
};

int CMenuExportsTemp::VID_GetWidth() const
{
	return vid.width;
};

int CMenuExportsTemp::VID_GetHeight() const
{
	return vid.height;
};

bool CMenuExportsTemp::VID_IsWindowed() const
{
#ifdef _WIN32
	return modestate == MS_WINDOWED;
#else
	// TODO
	return true;
#endif
};

int CMenuExportsTemp::VID_GetCurrentModeNum() const
{
	return vid_modenum;
};

int CMenuExportsTemp::VID_NumModes() const
{
	return ::VID_NumModes();
};

const char *CMenuExportsTemp::VID_GetModeDescription(int mode) const
{
	return ::VID_GetModeDescription(mode);
};

vmode_t *CMenuExportsTemp::VID_GetModePtr(int mode) const
{
	return ::VID_GetModePtr(mode);
};

void CMenuExportsTemp::SCR_BeginLoadingPlaque()
{
	::SCR_BeginLoadingPlaque();
};

int CMenuExportsTemp::SCR_ModalMessage(const char *msg)
{
	return ::SCR_ModalMessage(msg);
};

void CMenuExportsTemp::SCR_SetFullUpdateActive(bool abActive)
{
	scr_fullupdate = abActive;
};

void CMenuExportsTemp::SCR_SetCopyEverything(bool abActive)
{
	scr_copyeverything = abActive;
};

float CMenuExportsTemp::SCR_Con_Current() const
{
	return scr_con_current;
};

double CMenuExportsTemp::Host_GetTime() const
{
	return host_time;
};

void CMenuExportsTemp::Host_Quit_f()
{
	::Host_Quit_f();
};

const char *CMenuExportsTemp::Key_KeynumToString(int keynum)
{
	return ::Key_KeynumToString(keynum);
};

void CMenuExportsTemp::Key_SetBinding(int key, const char *cmd)
{
	::Key_SetBinding(key, cmd);
};

const char *CMenuExportsTemp::Key_GetBinding(int key) const
{
	return keybindings[key];
};

void CMenuExportsTemp::Con_ToggleConsole_f()
{
	::Con_ToggleConsole_f();
};

void CMenuExportsTemp::SetKeyDest(int nDest)
{
	key_dest = (keydest_t)nDest;
};

int CMenuExportsTemp::GetKeyDest() const
{
	return key_dest;
};

void CMenuExportsTemp::CL_SetDemoNum(int nNum)
{
	cls.demonum = nNum;
};

int CMenuExportsTemp::CL_GetDemoNum() const
{
	return cls.demonum;
};

bool CMenuExportsTemp::CL_DemoPlayback() const
{
	return cls.demoplayback;
};

bool CMenuExportsTemp::CL_IsConnected() const
{
	return cls.state == ca_connected;
};

void CMenuExportsTemp::CL_NextDemo()
{
	::CL_NextDemo();
};

int CMenuExportsTemp::CL_Intermission() const
{
	return cl.intermission;
};

double CMenuExportsTemp::Host_GetRealTime() const
{
	return realtime;
};

bool CMenuExportsTemp::Host_IsServerActive() const
{
	return sv.active;
};

int CMenuExportsTemp::GetMaxClients() const
{
	return svs.maxclients;
};

int CMenuExportsTemp::GetMaxClientsLimit() const
{
	return svs.maxclientslimit;
};

const char *CMenuExportsTemp::GetGameDir() const
{
	return com_gamedir;
};