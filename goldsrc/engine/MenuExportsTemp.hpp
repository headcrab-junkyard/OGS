/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2021 BlackPhrase
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

#pragma once

#include "IMenuExportsTemp.hpp"

class CMenuExportsTemp final : public IMenuExportsTemp
{
public:
	CMenuExportsTemp();
	~CMenuExportsTemp();
	
	void Cbuf_AddText(const char *text) override;
	void Cbuf_InsertText(const char *text) override;
	
	void Cmd_AddCommand(const char *name, xcommand_t function) override;
	
	void Cvar_Set(const char *name, const char *value) override;
	void Cvar_SetValue(const char *name, float value) override;
	const char *Cvar_VariableString(const char *name) const override;
	float Cvar_VariableValue(const char *name) const override;
	
	void S_LocalSound(const char *sample) override;
	
	void S_ExtraUpdate() override;
	
	void Draw_FadeScreen() override;
	void Draw_ConsoleBackground(int lines) override;
	qpic_t *Draw_CachePic(const char *name) override;
	void Draw_Character(int x, int y, int num) override;
	void Draw_Pic(int x, int y, qpic_t *pic) override;
	void Draw_TransPic(int x, int y, qpic_t *pic) override;
	void Draw_TransPicTranslate(int x, int y, qpic_t *pic, byte *translation) override;
	
	void VID_LockBuffer() override;
	void VID_UnlockBuffer() override;
	int VID_GetWidth() const override;
	int VID_GetHeight() const override;
	bool VID_IsWindowed() const override;
	int VID_GetCurrentModeNum() const override;
	int VID_NumModes() const override;
	const char *VID_GetModeDescription(int mode) const override;
	vmode_t *VID_GetModePtr(int mode) const override;
	
	void SCR_BeginLoadingPlaque() override;
	int SCR_ModalMessage(const char *msg) override;
	void SCR_SetFullUpdateActive(bool abActive) override;
	void SCR_SetCopyEverything(bool abActive) override;
	float SCR_Con_Current() const override;
	
	double Host_GetTime() const override;
	void Host_Quit_f() override;
	
	const char *Key_KeynumToString(int keynum) override;
	void Key_SetBinding(int key, const char *cmd) override;
	const char *Key_GetBinding(int key) const override;
	
	void Con_ToggleConsole_f() override;
	
	void SetKeyDest(int nDest) override;
	int GetKeyDest() const override;
	
	void CL_SetDemoNum(int nNum) override;
	int CL_GetDemoNum() const override;
	bool CL_DemoPlayback() const override;
	bool CL_IsConnected() const override;
	void CL_NextDemo() override;
	int CL_Intermission() const override;
	
	double Host_GetRealTime() const override;
	bool Host_IsServerActive() const override;
	
	int GetMaxClients() const override;
	int GetMaxClientsLimit() const override;
	
	const char *GetGameDir() const override;
};