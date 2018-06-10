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
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"
#include "IFileSystem.hpp"

IFileSystem *gpFileSystem{nullptr};

void FileSystem_Init(char *basedir, void *filesystemFactory)
{
	// TODO
	
	if(!basedir || !*basedir)
		return;
	
	if(!filesystemFactory)
		return;
	
	gpFileSystem = (IFileSystem*)(((CreateInterfaceFn)filesystemFactory)(OGS_FILESYSTEM_INTERFACE_VERSION, nullptr)); // TODO: FILESYSTEM_INTERFACE_VERSION
	
	if(!gpFileSystem)
		return;
};

void FileSystem_Shutdown()
{
	// TODO
	
	gpFileSystem = nullptr;
};

//////////////////////////////////////////////////////////////
// VFileSystem009 interface wrapper
//////////////////////////////////////////////////////////////

//void FS_Mount();
//void FS_Unmount();

void FS_RemoveAllSearchPaths()
{
	gpFileSystem->RemoveAllSearchPaths();
};

void FS_AddSearchPath(const char *pPath, const char *pathID)
{
	gpFileSystem->AddSearchPath(pPath, pathID);
};

bool FS_RemoveSearchPath(const char *pPath)
{
	return gpFileSystem->RemoveSearchPath(pPath);
};

void FS_RemoveFile(const char *pRelativePath, const char *pathID)
{
	gpFileSystem->RemoveFile(pRelativePath, pathID);
};

void FS_CreateDirHierarchy(const char *path, const char *pathID)
{
	gpFileSystem->CreateDirHierarchy(path, pathID);
};

bool FS_FileExists(const char *pFileName)
{
	return gpFileSystem->FileExists(pFileName);
};

bool FS_IsDirectory(const char *pFileName)
{
	return gpFileSystem->IsDirectory(pFileName);
};

FileHandle_t FS_Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	return gpFileSystem->Open(pFileName, pOptions, pathID);
};

void FS_Close(FileHandle_t file)
{
	gpFileSystem->Close(file);
};

void FS_Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	gpFileSystem->Seek(file, pos, seekType);
};

unsigned int FS_Tell(FileHandle_t file)
{
	return gpFileSystem->Tell(file);
};

unsigned int FS_Size(FileHandle_t file)
{
	return gpFileSystem->Size(file);
};

unsigned int FS_Size(const char *pFileName) // TODO: FS_FileSize? C doesn't support function overloading so this won't work otherwise
{
	return gpFileSystem->Size(pFileName);
};

long FS_GetFileTime(const char *pFileName)
{
	return gpFileSystem->GetFileTime(pFileName);
};

void FS_FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime)
{
	return gpFileSystem->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime);
};

bool FS_IsOk(FileHandle_t file)
{
	return gpFileSystem->IsOk(file);
};

void FS_Flush(FileHandle_t file)
{
	gpFileSystem->Flush(file);
};

bool FS_EndOfFile(FileHandle_t file)
{
	return gpFileSystem->EndOfFile(file);
};

int FS_Read(void *pOutput, int size, FileHandle_t file)
{
	return gpFileSystem->Read(pOutput, size, file);
};

int FS_Write(void const *pInput, int size, FileHandle_t file) // void const
{
	return gpFileSystem->Write(pInput, size, file);
};

char *FS_ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	return gpFileSystem->ReadLine(pOutput, maxChars, file);
};

int FS_FPrintf(FileHandle_t file, char *pFormat, ...)
{
	// TODO
	return gpFileSystem->FPrintf(file, pFormat);
};

void *FS_GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	return gpFileSystem->GetReadBuffer(file, outBufferSize, failIfNotInCache);
};

void FS_ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
	gpFileSystem->ReleaseReadBuffer(file, readBuffer);
};

const char *FS_FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return gpFileSystem->FindFirst(pWildCard, pHandle, pathID);
};

const char *FS_FindNext(FileFindHandle_t handle)
{
	return gpFileSystem->FindNext(handle);
};

bool FS_FindIsDirectory(FileFindHandle_t handle)
{
	return gpFileSystem->FindIsDirectory(handle);
};

void FS_FindClose(FileFindHandle_t handle)
{
	gpFileSystem->FindClose(handle);
};

void FS_GetLocalCopy(const char *pFileName)
{
	gpFileSystem->GetLocalCopy(pFileName);
};

const char *FS_GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return gpFileSystem->GetLocalPath(pFileName, pLocalPath, localPathBufferSize);
};

char *FS_ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted)
{
	return gpFileSystem->ParseFile(pFileBytes, pToken, pWasQuoted);
};

bool FS_FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	return gpFileSystem->FullPathToRelativePath(pFullpath, pRelative);
};

bool FS_GetCurrentDirectory(char *pDirectory, int maxlen)
{
	return gpFileSystem->GetCurrentDirectory(pDirectory, maxlen);
};

void FS_PrintOpenedFiles()
{
	gpFileSystem->PrintOpenedFiles();
};

void FS_SetWarningFunc(void (*pfnWarning)(const char *fmt, ...))
{
	gpFileSystem->SetWarningFunc(pfnWarning);
};

void FS_SetWarningLevel(FileWarningLevel_t level)
{
	gpFileSystem->SetWarningLevel(level);
};

void FS_LogLevelLoadStarted(const char *name)
{
	gpFileSystem->LogLevelLoadStarted(name);
};

void FS_LogLevelLoadFinished(const char *name)
{
	gpFileSystem->LogLevelLoadFinished(name);
};

int FS_HintResourceNeed(const char *hintlist, int forgetEverything)
{
	return gpFileSystem->HintResourceNeed(hintlist, forgetEverything);
};

int FS_PauseResourcePreloading()
{
	return gpFileSystem->PauseResourcePreloading();
};

int FS_ResumeResourcePreloading()
{
	return gpFileSystem->ResumeResourcePreloading();
};

int FS_SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	return gpFileSystem->SetVBuf(stream, buffer, mode, size);
};

void FS_GetInterfaceVersion(char *p, int maxlen)
{
	gpFileSystem->GetInterfaceVersion(p, maxlen);
};

bool FS_IsFileImmediatelyAvailable(const char *pFileName)
{
	return gpFileSystem->IsFileImmediatelyAvailable(pFileName);
};

WaitForResourcesHandle_t FS_WaitForResources(const char *resourcelist)
{
	return gpFileSystem->WaitForResources(resourcelist);
};

bool FS_GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress, bool *complete)
{
	return gpFileSystem->GetWaitForResourcesProgress(handle, progress, complete);
};

void FS_CancelWaitForResources(WaitForResourcesHandle_t handle)
{
	gpFileSystem->CancelWaitForResources(handle);
};

bool FS_IsAppReadyForOfflinePlay(int appID)
{
	return gpFileSystem->IsAppReadyForOfflinePlay(appID);
};

bool FS_AddPackFile(const char *fullpath, const char *pathID)
{
	return gpFileSystem->AddPackFile(fullpath, pathID);
};

FileHandle_t FS_OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID)
{
	return gpFileSystem->OpenFromCacheForRead(pFileName, pOptions, pathID);
};

void FS_AddSearchPathNoWrite(const char *pPath, const char *pathID)
{
	gpFileSystem->AddSearchPathNoWrite(pPath, pathID);
};

//////////////////////////////////////////////////////////////

/*
===============================================================================

FILE IO

===============================================================================
*/

int FS_FileOpenRead(const char *path, int *hndl)
{
	return 0; // TODO
};

int FS_FileOpenWrite(const char *path)
{
	return gpFileSystem->FileOpen(path, "wb");
};

void FS_FileClose(int handle)
{
	gpFileSystem->FileClose(handle);
};

void FS_FileSeek(int handle, int position)
{
	gpFileSystem->FileSeek(handle, position);
};

int FS_FileRead(int handle, void *dest, int count)
{
	return gpFileSystem->FileRead(handle, dest, count);
};

int FS_FileWrite(int handle, const void *data, int count)
{
	return gpFileSystem->FileWrite(handle, data, count);
};

int FS_FileTime(const char *path)
{
	return gpFileSystem->GetFileTime(path);
};

void FS_mkdir(const char *path)
{
	// TODO: if not dedicated?

#ifdef _WIN32
	//_mkdir(path); // TODO
#else
	mkdir(path, 0777);
#endif
};