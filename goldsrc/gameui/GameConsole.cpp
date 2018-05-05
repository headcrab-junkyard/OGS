/// @file

#include "GameConsole.hpp"

EXPOSE_SINGLE_INTERFACE(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION);

CGameConsole::CGameConsole() = default;
CGameConsole::~CGameConsole() = default;

void CGameConsole::Activate()
{
};

void CGameConsole::Initialize()
{
};

void CGameConsole::Hide()
{
};

void CGameConsole::Clear()
{
};

bool CGameConsole::IsConsoleVisible()
{
};

void CGameConsole::Printf(const char *format, ...)
{
};

void CGameConsole::DPrintf(const char *format, ...)
{
};

void CGameConsole::SetParent(vgui2::VPANEL parent)
{
};