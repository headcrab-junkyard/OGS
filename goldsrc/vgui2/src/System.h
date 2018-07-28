/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2018 BlackPhrase
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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "vgui2/ISystem.h"

namespace vgui2
{
class CSystem : public ISystem
{
public:
	void Shutdown() override;

	void RunFrame() override;

	void ShellExecute(const char *command, const char *file) override;

	double GetFrameTime() override;
	double GetCurrentTime() override;

	long GetTimeMillis() override;

	int GetClipboardTextCount() override;

	void SetClipboardText(const char *text, int textLen) override;
	void SetClipboardText(const wchar_t *text, int textLen) override;
	int GetClipboardText(int offset, char *buf, int bufLen) override;
	int GetClipboardText(int offset, wchar_t *buf, int bufLen) override;

	bool SetRegistryString(const char *key, const char *value) override;
	bool GetRegistryString(const char *key, char *value, int valueLen) override;
	bool SetRegistryInteger(const char *key, int value) override;
	bool GetRegistryInteger(const char *key, int &value) override;

	KeyValues *GetUserConfigFileData(const char *dialogName, int dialogID) override;
	void SetUserConfigFile(const char *fileName, const char *pathName) override;
	void SaveUserConfigFile() override;

	bool SetWatchForComputerUse(bool state) override;

	double GetTimeSinceLastUse() override;

	int GetAvailableDrives(char *buf, int bufLen) override;

	bool CommandLineParamExists(const char *paramName) override;
	const char *GetFullCommandLine() override;

	bool GetCurrentTimeAndDate(int *year, int *month, int *dayOfWeek, int *day, int *hour, int *minute, int *second) override;

	double GetFreeDiskSpace(const char *path) override;

	bool CreateShortcut(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory, const char *iconFile) override;

	bool GetShortcutTarget(const char *linkFileName, char *targetPath, char *arguments, int destBufferSizes) override;
	bool ModifyShortcutTarget(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory) override;

	bool GetCommandLineParamValue(const char *paramName, char *value, int valueBufferSize) override;

	bool DeleteRegistryKey(const char *keyName) override;

	const char *GetDesktopFolderPath() override;
};

}; // namespace vgui2