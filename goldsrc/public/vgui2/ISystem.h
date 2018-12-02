/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

#include "VGUI.h"
#include "KeyCode.h"
#include "interface.h"

#ifdef PlaySound
#undef PlaySound
#endif

const char VGUI_SYSTEM_INTERFACE_VERSION[] = "VGUI_System009";

class KeyValues;

namespace vgui2
{

struct ISystem : public IBaseInterface
{
	///
	virtual void Shutdown() = 0;
	
	///
	virtual void RunFrame() = 0;
	
	///
	virtual void ShellExecute(const char *sCommand, const char *sFile) = 0;
	
	///
	virtual double GetFrameTime() = 0;
	
	///
	virtual double GetCurrentTime() = 0;
	
	///
	virtual long GetTimeMillis() = 0;
	
	///
	virtual int GetClipboardTextCount() = 0;
	
	///
	virtual void SetClipboardText(const char *sText, int nTextLen) = 0;
	
	///
	virtual void SetClipboardText(const wchar_t *sText, int nTextLen) = 0;
	
	///
	virtual int GetClipboardText(int nOffset, char *buf, int nBufLen) = 0;
	
	///
	virtual int GetClipboardText(int nOffset, wchar_t *buf, int nBufLen) = 0;
	
	///
	virtual bool SetRegistryString(const char *sKey, const char *sValue) = 0;
	
	///
	virtual bool GetRegistryString(const char *sKey, char *sValue, int nValueLen) = 0;
	
	///
	virtual bool SetRegistryInteger(const char *sKey, int nValue) = 0;
	
	///
	virtual bool GetRegistryInteger(const char *sKey, int &value) = 0;
	
	///
	virtual KeyValues *GetUserConfigFileData(const char *sDialogName, int nDialogID) = 0;
	
	///
	virtual void SetUserConfigFile(const char *sFileName, const char *sPathName) = 0;
	
	///
	virtual void SaveUserConfigFile() = 0;
	
	///
	virtual bool SetWatchForComputerUse(bool bState) = 0;
	
	///
	virtual double GetTimeSinceLastUse() = 0;
	
	///
	virtual int GetAvailableDrives(char *buf, int bufLen) = 0;
	
	///
	virtual bool CommandLineParamExists(const char *sParamName) = 0;
	
	///
	virtual const char *GetFullCommandLine() = 0;
	
	///
	virtual bool GetCurrentTimeAndDate(int *year, int *month, int *dayOfWeek, int *day, int *hour, int *minute, int *second) = 0;
	
	///
	virtual double GetFreeDiskSpace(const char *sPath) = 0;
	
	///
	virtual bool CreateShortcut(const char *sLinkFileName, const char *sTargetPath, const char *sArguments, const char *sWorkingDirectory, const char *sIconFile) = 0;
	
	///
	virtual bool GetShortcutTarget(const char *sLinkFileName, char *targetPath, char *arguments, int destBufferSizes) = 0;
	
	///
	virtual bool ModifyShortcutTarget(const char *sLinkFileName, const char *sTargetPath, const char *sArguments, const char *sWorkingDirectory) = 0;
	
	///
	virtual bool GetCommandLineParamValue(const char *sParamName, char *value, int valueBufferSize) = 0;
	
	///
	virtual bool DeleteRegistryKey(const char *sKeyName) = 0;
	
	///
	virtual const char *GetDesktopFolderPath() = 0;
};

}; // namespace vgui2