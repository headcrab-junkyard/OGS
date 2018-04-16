/// @file

#pragma once

#include "GameUI/IGameConsole.h"

class CGameConsole final : public IGameConsole
{
public:
	CGameConsole();
	~CGameConsole();

	void Activate() override;

	void Initialize() override;

	void Hide() override;

	void Clear() override;

	bool IsConsoleVisible() override;

	void Printf(const char *format, ...) override;
	void DPrintf(const char *format, ...) override;

	void SetParent(vgui2::VPANEL parent) override;
};