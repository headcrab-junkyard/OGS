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
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "System.h"

namespace vgui2
{

void CSystem::Shutdown()
{
};

void CSystem::RunFrame()
{
};

void CSystem::ShellExecute(const char *command, const char *file)
{
};

double CSystem::GetFrameTime()
{
	return 0.0;
};

double CSystem::GetCurrentTime()
{
	return 0.0;
};

long CSystem::GetTimeMillis()
{
	return 0;
};

int CSystem::GetClipboardTextCount()
{
	return 0;
};

void CSystem::SetClipboardText(const char *text, int textLen)
{
};

void CSystem::SetClipboardText(const wchar_t *text, int textLen)
{
};

int CSystem::GetClipboardText(int offset, char *buf, int bufLen)
{
	return 0;
};

int CSystem::GetClipboardText(int offset, wchar_t *buf, int bufLen)
{
	return 0;
};

bool CSystem::SetRegistryString(const char *key, const char *value)
{
	return false;
};

bool CSystem::GetRegistryString(const char *key, char *value, int valueLen)
{
	return false;
};

bool CSystem::SetRegistryInteger(const char *key, int value)
{
	return false;
};

bool CSystem::GetRegistryInteger(const char *key, int &value)
{
	return false;
};

KeyValues *CSystem::GetUserConfigFileData(const char *dialogName, int dialogID)
{
	return nullptr;
};

void CSystem::SetUserConfigFile(const char *fileName, const char *pathName)
{
};

void CSystem::SaveUserConfigFile()
{
};

bool CSystem::SetWatchForComputerUse(bool state)
{
	return false;
};

double CSystem::GetTimeSinceLastUse()
{
	return 0.0;
};

int CSystem::GetAvailableDrives(char *buf, int bufLen)
{
	return 0;
};

bool CSystem::CommandLineParamExists(const char *paramName)
{
	return false;
};

const char *CSystem::GetFullCommandLine()
{
	return "";
};

bool CSystem::GetCurrentTimeAndDate(int *year, int *month, int *dayOfWeek, int *day, int *hour, int *minute, int *second)
{
	return false;
};

double CSystem::GetFreeDiskSpace(const char *path)
{
	return 0.0;
};

bool CSystem::CreateShortcut(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory, const char *iconFile)
{
	return false;
};

bool CSystem::GetShortcutTarget(const char *linkFileName, char *targetPath, char *arguments, int destBufferSizes)
{
	return false;
};

bool CSystem::ModifyShortcutTarget(const char *linkFileName, const char *targetPath, const char *arguments, const char *workingDirectory)
{
	return false;
};

bool CSystem::GetCommandLineParamValue(const char *paramName, char *value, int valueBufferSize)
{
	return false;
};

bool CSystem::DeleteRegistryKey(const char *keyName)
{
	return false;
};

const char *CSystem::GetDesktopFolderPath()
{
	return "";
};

}; // namespace vgui2