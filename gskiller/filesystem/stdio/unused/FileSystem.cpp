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

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include "FileSystem.hpp"

#ifdef _DEBUG
static FILE *ghLogFile = nullptr;

void TRACE_LOG(const char *asMsg, ...)
{
	if(!ghLogFile)
		return;

	va_list ArgList;
	char sMsg[512];

	va_start(ArgList, asMsg);
	vsnprintf(sMsg, sizeof(sMsg), asMsg, ArgList);
	va_end(ArgList);

	sprintf(sMsg, "%s\n", sMsg);
	fputs(sMsg, ghLogFile);
	fflush(ghLogFile);
};
#else
void TRACE_LOG(const char *asMsg, ...){};
#endif

static CFileSystem gFileSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION, gFileSystem);

CFileSystem::CFileSystem() = default;
CFileSystem::~CFileSystem() = default;

void CFileSystem::Mount()
{
#ifdef _DEBUG
	ghLogFile = fopen("TRACE.log", "w+");
#endif

	TRACE_LOG("CFileSystem::Mount");
};

void CFileSystem::Unmount()
{
	TRACE_LOG("CFileSystem::Unmount");

#ifdef _DEBUG
	if(ghLogFile)
		fclose(ghLogFile);
#endif
};

void CFileSystem::RemoveAllSearchPaths()
{
	TRACE_LOG("CFileSystem::RemoveAllSearchPaths");
};

void CFileSystem::AddSearchPath(const char *pPath, const char *pathID)
{
	TRACE_LOG("CFileSystem::AddSearchPath(%s, %s)", pPath, pathID);
};

bool CFileSystem::RemoveSearchPath(const char *pPath)
{
	TRACE_LOG("CFileSystem::RemoveSearchPath(%s)", pPath);
	return false;
};

void CFileSystem::RemoveFile(const char *pRelativePath, const char *pathID)
{
	TRACE_LOG("CFileSystem::RemoveFile(%s, %s)", pRelativePath, pathID);
};

void CFileSystem::CreateDirHierarchy(const char *path, const char *pathID)
{
	TRACE_LOG("CFileSystem::CreateDirHierarchy(%s, %s)", path, pathID);
};

bool CFileSystem::FileExists(const char *pFileName)
{
	TRACE_LOG("CFileSystem::FileExists(%s)", pFileName);
	return false;
};

bool CFileSystem::IsDirectory(const char *pFileName)
{
	TRACE_LOG("CFileSystem::IsDirectory(%s)", pFileName);
	return false;
};

FileHandle_t CFileSystem::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE_LOG("CFileSystem::Open(%s, %s, %s)", pFileName, pOptions, pathID);
	return nullptr;
};

void CFileSystem::Close(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Close(%d)", file);
	fclose((FILE*)file);
};

void CFileSystem::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	TRACE_LOG("CFileSystem::Seek(%d, %d, %d)", file, pos, seekType);
	lseek((FILE*)file, pos, SEEK_SET);
};

unsigned int CFileSystem::Tell(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Tell(%d)", file);
	return 0;
};

unsigned int CFileSystem::Size(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Size(%d)", file);
	return 0;
};

unsigned int CFileSystem::Size(const char *pFileName)
{
	TRACE_LOG("CFileSystem::Size(%s)", pFileName);
	return 0;
};

/*
============
Sys_FileTime

returns -1 if not present
============
*/
long CFileSystem::GetFileTime(const char *pFileName)
{
	TRACE_LOG("CFileSystem::GetFileTime(%s)", pFileName);
	
	struct stat buf;

	if(stat(pFileName, &buf) == -1)
		return -1;

	return buf.st_mtime;
};

void CFileSystem::FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime)
{
	TRACE_LOG("CFileSystem::FileTimeToString(%s, %d, %l", pStrip, maxCharsIncludingTerminator, fileTime);
};

bool CFileSystem::IsOk(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::IsOk(%d)", file);
	return false;
};

void CFileSystem::Flush(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Flush(%d)", file);
	fflush((FILE*)file);
};

bool CFileSystem::EndOfFile(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::EndOfFile(%d)", file);
	return eof((FILE*)file);
};

int CFileSystem::Read(void *pOutput, int size, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Read(%p, %d, %d)", pOutput, size, file);
	return fread((FILE*)file, pOutput, size);
};

int CFileSystem::Write(void const *pInput, int size, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Write(%p, %d, %d)", pInput, size, file);
	return fwrite((FILE*)file, pInput, size);
};

char *CFileSystem::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::ReadLine(%s, %d, %d)", pOutput, maxChars, file);
	return nullptr;
};

int CFileSystem::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	TRACE_LOG("CFileSystem::FPrintf(%d, %s)", file, pFormat);
	return 0;
};

void *CFileSystem::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	TRACE_LOG("CFileSystem::GetReadBuffer");
	return nullptr;
};

void CFileSystem::ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
	TRACE_LOG("CFileSystem::ReleaseReadBuffer(%d, %p)", file, readBuffer);
};

const char *CFileSystem::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	TRACE_LOG("CFileSystem::FindFirst(%s, %p, %d)", pWildCard, pHandle, pathID);
	return Sys_FindFirst(pathID, 0, 0);
};

const char *CFileSystem::FindNext(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindNext(%d)", handle);
	return Sys_FindNext(handle, 0, 0);
};

bool CFileSystem::FindIsDirectory(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindIsDirectory(%d)", handle);
	return false;
};

void CFileSystem::FindClose(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindClose(%d)", handle);
	Sys_FindClose(handle);
};

void CFileSystem::GetLocalCopy(const char *pFileName)
{
	TRACE_LOG("CFileSystem::GetLocalCopy(%s)", pFileName);
};

const char *CFileSystem::GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	TRACE_LOG("CFileSystem::GetLocalPath(%s, %s, %d)", pFileName, pLocalPath, localPathBufferSize);
	return "";
};

char *CFileSystem::ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted)
{
	TRACE_LOG("CFileSystem::ParseFile(%s, %s, %p", pFileBytes, pToken, pWasQuoted);
	return nullptr;
};

bool CFileSystem::FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	TRACE_LOG("CFileSystem::FullPathToRelativePath(%s, %s)", pFullpath, pRelative);
	return false;
};

bool CFileSystem::GetCurrentDirectory(char *pDirectory, int maxlen)
{
	TRACE_LOG("CFileSystem::GetCurrentDirectory(%s, %d)", pDirectory, maxlen);
	return false;
};

void CFileSystem::PrintOpenedFiles()
{
	TRACE_LOG("CFileSystem::PrintOpenedFiles()");
};

void CFileSystem::SetWarningFunc(void (*pfnWarning)(const char *fmt,  ...))
{
	TRACE_LOG("CFileSystem::SetWarningFunc");
};

void CFileSystem::SetWarningLevel(FileWarningLevel_t level)
{
	TRACE_LOG("CFileSystem::SetWarningLevel(%d)", level);
};

void CFileSystem::LogLevelLoadStarted(const char *name)
{
	TRACE_LOG("CFileSystem::LogLevelLoadStarted(%s)", name);
};

void CFileSystem::LogLevelLoadFinished(const char *name)
{
	TRACE_LOG("CFileSystem::LogLevelLoadFinished(%s)", name);
};

int CFileSystem::HintResourceNeed(const char *hintlist, int forgetEverything)
{
	TRACE_LOG("CFileSystem::HintResourceNeed(%s, %d)", hintlist, forgetEverything);
	return 0;
};

int CFileSystem::PauseResourcePreloading()
{
	TRACE_LOG("CFileSystem::PauseResourcePreloading()");
	return 0;
};

int CFileSystem::ResumeResourcePreloading()
{
	TRACE_LOG("CFileSystem::ResumeResourcePreloading()");
	return 0;
};

int CFileSystem::SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	TRACE_LOG("CFileSystem::SetVBuf(%d, %s, %d, %l)", stream, buffer, mode, size);
	return 0;
};

void CFileSystem::GetInterfaceVersion(char *p, int maxlen)
{
	TRACE_LOG("CFileSystem::GetInterfaceVersion(%s, %d)", p, maxlen);
};

bool CFileSystem::IsFileImmediatelyAvailable(const char *pFileName)
{
	TRACE_LOG("CFileSystem::IsFileImmediatelyAvailable(%s)", pFileName);
	return false;
};

WaitForResourcesHandle_t CFileSystem::WaitForResources(const char *resourcelist)
{
	TRACE_LOG("CFileSystem::WaitForResources(%s)", resourcelist);
	return 0;
};

bool CFileSystem::GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */)
{
	TRACE_LOG("CFileSystem::GetWaitForResourcesProgress");
	return false;
};

void CFileSystem::CancelWaitForResources(WaitForResourcesHandle_t handle)
{
	TRACE_LOG("CFileSystem::CancelWaitForResources(%d)", handle);
};

bool CFileSystem::IsAppReadyForOfflinePlay(int appID)
{
	TRACE_LOG("CFileSystem::IsAppReadyForOfflinePlay(%d)", appID);
	return false;
};

bool CFileSystem::AddPackFile(const char *fullpath, const char *pathID)
{
	TRACE_LOG("CFileSystem::AddPackFile(%s, %s)", fullpath, pathID);
	return false;
};

FileHandle_t CFileSystem::OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE_LOG("CFileSystem::OpenFromCacheForRead(%s, %s, %s)", pFileName, pOptions, pathID);
	return nullptr;
};

void CFileSystem::AddSearchPathNoWrite(const char *pPath, const char *pathID)
{
	TRACE_LOG("CFileSystem::AddSearchPathNoWrite(%s, %s)", pPath, pathID);
};