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

#include "ContentControlDialog.hpp"

CContentControlDialog::CContentControlDialog(vgui::Panel *apParent)
	: vgui::Frame(apParent, "ContentControlDialog")
{
	SetBounds(0, 0, 372, 160);
	SetSizeable(false);
	
	SetTitle("#GameUI_ContentLock", true);
	
	mpStatusLabel = vgui::Label(this, "ContentStatus", "");
	
	mpPasswordLabel = new vgui::Label(this, "PasswordPrompt", "#GameUI_PasswordPrompt");
	mpPassword2Label = new vgui::Label(this, "PasswordReentryPrompt", "#GameUI_PasswordReentryPrompt");
	
	mpExplainLabel = new vgui::Label(this, "ContentControlExplain", "");
	
	mpPassword = new vgui::TextEntry(this, "Password");
	mpPassword2 = new vgui::TextEntry(this, "Password2");
	
	mpOKButton = new vgui::Button(this, "Ok", "#GameUI_OK");
	mpOKButton->SetCommand("Ok");
	
	auto pCancelButton{new vgui::Button(this, "Cancel", "#GameUI_Cancel")};
	pCancelButton->SetCommand("Cancel");
	
	msGorePW[0] = '\0';
	ResetPassword();
	
	LoadControlSettings("resource\\ContentControlDialog.res"); // TODO: Resource?
	
	//Explain("");
	//UpdateContentControlStatus();
};

CContentControlDialog::~CContentControlDialog() = default;

void CContentControlDialog::OnCommand(const char *asCmd)
{
	if(!stricmp(asCmd, "Ok"))
	{
		bool bCanClose{false};
		
		char sPass1[256]{};
		char sPass2[256]{};
		
		mpPassword->GetText(sPass1, sizeof(sPass1));
		mpPassword2->GetText(sPass2, sizeof(sPass2));
		
		if(IsPasswordEnabledInDialog())
			bCanClose = DisablePassword(sPass1);
		else if(!strcmp(sPass1, sPass2))
			bCanClose = EnablePassword(sPass1);
		else
			Explain("#GameUI_PasswordsDontMatch");
		
		if(bCanClose)
			OnClose();
	}
	else if(!stricmp(asCmd, "Cancel"))
		OnClose();
	else
		BaseClass::OnCommand(asCmd);
};

void CContentControlDialog::OnClose()
{
	BaseClass::OnClose();
	PostActionSignal(new KeyValeues("ContentControlClose"));
	//MarkForDeletion();
};

void CContentControlDialog::Activate()
{
	BaseClass::Activate();
	
	mpPassword->SetText("");
	mpPassword->RequestFocus();
	
	mpPassword2->SetText("");
	
	Explain("");
	UpdateContentControlStatus();
	
	GetInput()->SetAppModalSurface(GetVPanel());
};

void CContentControlDialog::ResetPassword()
{
	// Set initial value
	if(false)
		;
	else
		msGorePW[0] = '\0';
};

void CContentControlDialog::ApplyPassword()
{
	WriteToken(msGorePW);
};

void CContentControlDialog::WriteToken(const char *asToken)
{
};

void CContentControlDialog::Explain(const char *asMsg, ...)
{
	if(!mpExplainLabel)
		return;
	
	va_list argptr;
	char sText[1024]{};
	
	va_start(argptr, asMsg);
	Q_vsnprintf(sText, sizeof(sText), asMsg, argptr);
	va_end(argptr);
	
	mpExplainLabel->SetText(sText);
};

bool CContentControlDialog::CheckPassword(const char *asOldPassword, const char *asNewPassword, bool abEnableContentControl)
{
};

bool CContentControlDialog::EnablePassword(const char *asNewPassword)
{
	if(!asNewPassword[0])
	{
		Explain("#GameUI_MustEnterPassword");
		return false;
	};
	
	char sHashedPW[128]{};
	HashPassword(asNewPassword, sHashedPW, sizeof(sHashedPW));
	
	// Disable violence
	
/*
	gpEngine->Cvar_SetValue("violence_hblood", 0.0);
	gpEngine->Cvar_SetValue("violence_hgibs", 0.0);
	gpEngine->Cvar_SetValue("violence_ablood", 0.0);
	gpEngine->Cvar_SetValue("violence_agibs", 0.0);
*/
	
	ConVarRef violence_hblood("violence_hblood");
	violence_hblood.SetValue(false);
	
	ConVarRef violence_hgibs("violence_hgibs");
	violence_hgibs.SetValue(false);
	
	ConVarRef violence_ablood("violence_ablood");
	violence_ablood.SetValue(false);
	
	ConVarRef violence_agibs("violence_agibs");
	violence_agibs.SetValue(false);
	
	// Store digest to registry
	//WriteToken(sHashedPW);
	Q_strncpy(msGorePW, sHashedPW, sizeof(msGorePW));
	
	return true;
};

bool CContentControlDialog::DisablePassword(const char *asOldPassword)
{
	if(!asOldPassword[0])
	{
		Explain("#GameUI_MustEnterPassword");
		return false;
	};
	
	char sHashedPW[128]{};
	HashPassword(asOldPassword, sHashedPW, sizeof(sHashedPW));
	
	if(stricmp(msGorePW, sHashedPW))
	{
		Explain("#GameUI_IncorrectPassword");
		return false;
	};
	
	msGorePW[0] = '\0';
	
	// Enable violence
	
/*
	gpEngine->Cvar_SetValue("violence_hblood", 1.0);
	gpEngine->Cvar_SetValue("violence_hgibs", 1.0);
	gpEngine->Cvar_SetValue("violence_ablood", 1.0);
	gpEngine->Cvar_SetValue("violence_agibs", 1.0);
*/

	ConVarRef violence_hblood("violence_hblood");
	violence_hblood.SetValue(true);
	
	ConVarRef violence_hgibs("violence_hgibs");
	violence_hgibs.SetValue(true);
	
	ConVarRef violence_ablood("violence_ablood");
	violence_ablood.SetValue(true);
	
	ConVarRef violence_agibs("violence_agibs");
	violence_agibs.SetValue(true);
	
	// Remove hashed value
	//WriteToken("");
	
	return true;
};

void CContentControlDialog::HashPassword(const char *asNewPassword, char *asHashBuf, int anMaxLen)
{
};

void CContentControlDialog::UpdateContentControlStatus()
{
	bool bEnabled{IsPasswordEnabledInDialog()};
	
	mpStatusLabel->SetText(bEnabled ? "#GameUI_ContentStatusEnabled" : "#GameUI_ContentStatusDisabled");
	
	if(bEnabled)
		mpPasswordLabel->SetText("#GameUI_PasswordDisablePrompt");
	else
		mpPasswordLabel->SetText("#GameUI_PasswordPrompt");
	
	// Hide the re-entry field and label
	mpPassword2Label->SetVisible(!bEnabled);
	mpPassword2-SetVisible(!bEnabled);
	mpOKButton->SetText(bEnabled ? "#GameUI_Disable" : "#GameUI_Enable");
};