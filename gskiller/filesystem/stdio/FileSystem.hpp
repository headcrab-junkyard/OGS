/*
 *	This file is part of Open GoldSrc Project
 *	Copyright (C) 2016-2018 Headcrab Garage
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of this program with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve"). You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve. If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so. If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file
/// @brief IFileSystem interface implementation

#pragma once

#include "public/FileSystem.h"

class CFileSystem final : public IFileSystem
{
public:
	CFileSystem();
	~CFileSystem();

	void Mount() override;
	void Unmount() override;

	void RemoveAllSearchPaths() override;

	void AddSearchPath(const char *pPath, const char *pathID) override;
	bool RemoveSearchPath(const char *pPath) override;

	void RemoveFile(const char *pRelativePath, const char *pathID) override;

	void CreateDirHierarchy(const char *path, const char *pathID) override;

	bool FileExists(const char *pFileName) override;
	bool IsDirectory(const char *pFileName) override;

	FileHandle_t Open(const char *pFileName, const char *pOptions, const char *pathID) override;
	void Close(FileHandle_t file) override;

	void Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType) override;
	unsigned int Tell(FileHandle_t file) override;

	unsigned int Size(FileHandle_t file) override;
	unsigned int Size(const char *pFileName) override;

	long GetFileTime(const char *pFileName) override;
	void FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime) override;

	bool IsOk(FileHandle_t file) override;

	void Flush(FileHandle_t file) override;
	bool EndOfFile(FileHandle_t file) override;

	int Read(void *pOutput, int size, FileHandle_t file) override;
	int Write(void const *pInput, int size, FileHandle_t file) override; // void const
	char *ReadLine(char *pOutput, int maxChars, FileHandle_t file) override;
	int FPrintf(FileHandle_t file, char *pFormat, ...) override;

	void *GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache) override;
	void ReleaseReadBuffer(FileHandle_t file, void *readBuffer) override;

	const char *FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID) override;
	const char *FindNext(FileFindHandle_t handle) override;
	bool FindIsDirectory(FileFindHandle_t handle) override;
	void FindClose(FileFindHandle_t handle) override;

	void GetLocalCopy(const char *pFileName) override;

	const char *GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize) override;

	char *ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted) override;

	bool FullPathToRelativePath(const char *pFullpath, char *pRelative) override;

	bool GetCurrentDirectory(char *pDirectory, int maxlen) override;

	void PrintOpenedFiles() override;

	void SetWarningFunc(void (*pfnWarning)(const char *fmt, ...)) override;
	void SetWarningLevel(FileWarningLevel_t level) override;

	void LogLevelLoadStarted(const char *name) override;
	void LogLevelLoadFinished(const char *name) override;
	
	int HintResourceNeed(const char *hintlist, int forgetEverything) override;
	
	int PauseResourcePreloading() override;
	int ResumeResourcePreloading() override;
	
	int SetVBuf(FileHandle_t stream, char *buffer, int mode, long size) override;
	
	void GetInterfaceVersion(char *p, int maxlen) override;
	
	bool IsFileImmediatelyAvailable(const char *pFileName) override;

	WaitForResourcesHandle_t WaitForResources(const char *resourcelist) override;
	bool GetWaitForResourcesProgress(WaitForResourcesHandle_t handle,
	                                 float *progress /* out */,
	                                 bool *complete /* out */) override;
	void CancelWaitForResources(WaitForResourcesHandle_t handle) override;

	bool IsAppReadyForOfflinePlay(int appID) override;

	bool AddPackFile(const char *fullpath, const char *pathID) override;

	FileHandle_t OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID) override;

	void AddSearchPathNoWrite(const char *pPath, const char *pathID) override;
};