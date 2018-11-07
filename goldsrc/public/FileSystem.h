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
/// @brief filesystem interface

#pragma once

#include "interface.h"

const char FILESYSTEM_INTERFACE_VERSION[] = "VFileSystem009";

using FileHandle_t = void *;
using FileFindHandle_t = int;
using WaitForResourcesHandle_t = int;

// If in engine space skip this definitions
#ifndef FILESYSTEM_INTERNAL_H

#define FILESYSTEM_INVALID_HANDLE (FileHandle_t)0

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD = 0,
	FILESYSTEM_SEEK_CURRENT,
	FILESYSTEM_SEEK_TAIL
	//...?
};

enum
{
	FILESYSTEM_INVALID_FIND_HANDLE = -1
};

enum FileWarningLevel_t
{
	FILESYSTEM_WARNING_QUIET = 0,      ///< Don't print anything
	FILESYSTEM_WARNING_REPORTUNCLOSED, ///< On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUSAGE,    ///< Report number of times a file was opened/closed
	FILESYSTEM_WARNING_REPORTALLACCESS ///< Report all open/close events to console (SLOW!!!!)
};

#endif // FILESYSTEM_INTERNAL_H

#undef GetCurrentDirectory

struct IFileSystem : public IBaseInterface
{
	///
	virtual void Mount() = 0;

	///
	virtual void Unmount() = 0;

	/// Remove all search paths (including write path)
	virtual void RemoveAllSearchPaths() = 0;

	///
	virtual void AddSearchPath(const char *pPath, const char *pathID) = 0;

	///
	virtual bool RemoveSearchPath(const char *pPath) = 0;

	///
	virtual void RemoveFile(const char *pRelativePath, const char *pathID) = 0;

	///
	virtual void CreateDirHierarchy(const char *path, const char *pathID) = 0;

	///
	virtual bool FileExists(const char *pFileName) = 0;

	///
	virtual bool IsDirectory(const char *pFileName) = 0;

	///
	virtual FileHandle_t Open(const char *pFileName, const char *pOptions, const char *pathID) = 0;

	///
	virtual void Close(FileHandle_t file) = 0;

	///
	virtual void Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType) = 0;

	///
	virtual unsigned int Tell(FileHandle_t file) = 0;

	///
	virtual unsigned int Size(FileHandle_t file) = 0;

	///
	virtual unsigned int Size(const char *pFileName) = 0;

	///
	virtual long GetFileTime(const char *pFileName) = 0;

	///
	virtual void FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime) = 0;

	///
	virtual bool IsOk(FileHandle_t file) = 0;

	///
	virtual void Flush(FileHandle_t file) = 0;

	///
	virtual bool EndOfFile(FileHandle_t file) = 0;

	///
	virtual int Read(void *pOutput, int size, FileHandle_t file) = 0;

	///
	virtual int Write(void const *pInput, int size, FileHandle_t file) = 0; // void const

	///
	virtual char *ReadLine(char *pOutput, int maxChars, FileHandle_t file) = 0;

	///
	virtual int FPrintf(FileHandle_t file, char *pFormat, ...) = 0;

	///
	virtual void *GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache) = 0;

	///
	virtual void ReleaseReadBuffer(FileHandle_t file, void *readBuffer) = 0;

	///
	virtual const char *FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID) = 0;

	///
	virtual const char *FindNext(FileFindHandle_t handle) = 0;

	///
	virtual bool FindIsDirectory(FileFindHandle_t handle) = 0;

	///
	virtual void FindClose(FileFindHandle_t handle) = 0;

	///
	virtual void GetLocalCopy(const char *pFileName) = 0;

	///
	virtual const char *GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize) = 0;

	///
	virtual char *ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted) = 0;

	///
	virtual bool FullPathToRelativePath(const char *pFullpath, char *pRelative) = 0;

	///
	virtual bool GetCurrentDirectory(char *pDirectory, int maxlen) = 0;

	///
	virtual void PrintOpenedFiles() = 0;

	///
	virtual void SetWarningFunc(void (*pfnWarning)(const char *fmt, ...)) = 0;

	///
	virtual void SetWarningLevel(FileWarningLevel_t level) = 0;

	///
	virtual void LogLevelLoadStarted(const char *name) = 0;

	///
	virtual void LogLevelLoadFinished(const char *name) = 0;

	///
	virtual int HintResourceNeed(const char *hintlist, int forgetEverything) = 0;

	///
	virtual int PauseResourcePreloading() = 0;

	///
	virtual int ResumeResourcePreloading() = 0;

	///
	virtual int SetVBuf(FileHandle_t stream, char *buffer, int mode, long size) = 0;

	///
	virtual void GetInterfaceVersion(char *p, int maxlen) = 0;

	///
	virtual bool IsFileImmediatelyAvailable(const char *pFileName) = 0;

	///
	virtual WaitForResourcesHandle_t WaitForResources(const char *resourcelist) = 0;

	///
	virtual bool GetWaitForResourcesProgress(WaitForResourcesHandle_t handle,
	                                 float *progress /* out */,
	                                 bool *complete /* out */) = 0;

	///
	virtual void CancelWaitForResources(WaitForResourcesHandle_t handle) = 0;

	///
	virtual bool IsAppReadyForOfflinePlay(int appID) = 0;

	///
	virtual bool AddPackFile(const char *fullpath, const char *pathID) = 0;

	///
	virtual FileHandle_t OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID) = 0;

	/// Adds a read-only search path
	/// @see AddSearchPath
	virtual void AddSearchPathNoWrite(const char *pPath, const char *pathID) = 0;
};