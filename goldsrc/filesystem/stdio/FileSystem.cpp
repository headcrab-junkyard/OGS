/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2016-2019 BlackPhrase
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

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include "FileSystem.hpp"

// TODO
#define MAX_QPATH 64   // max length of a quake game pathname
#define MAX_OSPATH 128 // max length of a filesystem pathname

int com_filesize{0};

char com_cachedir[MAX_OSPATH]{};

//
// in memory
//

typedef struct
{
	char name[MAX_QPATH]{};
	int filepos{0}, filelen{0};
} packfile_t;

typedef struct pack_s
{
	char filename[MAX_OSPATH]{};
	FileHandle_t handle{nullptr};
	int numfiles{0};
	packfile_t *files{nullptr};
} pack_t;

//
// on disk
//

typedef struct
{
	char name[56]{};
	int filepos{0}, filelen{0};
} dpackfile_t;

typedef struct
{
	char id[4]{};
	int dirofs{0};
	int dirlen{0};
} dpackheader_t;

#define MAX_FILES_IN_PACK 2048

typedef struct searchpath_s
{
	char filename[MAX_OSPATH]{};
	pack_t *pack{nullptr}; ///< only one of filename / pack will be used
	struct searchpath_s *next{nullptr};
} searchpath_t;

searchpath_t *com_searchpaths{nullptr};

/*
============
COM_Path_f

============
*/
/*
void COM_Path_f()
{
	Con_Printf("Current search path:\n");
	for(auto *s = com_searchpaths; s; s = s->next)
	{
		if(s->pack)
			Con_Printf("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
		else
			Con_Printf("%s\n", s->filename);
	};
};
*/

/*
================
filelength
================
*/
int /*CFileSystem::*/filelength(FILE *f)
{
	int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	
	int end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
};

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
	
	//Cmd_AddCommand("path", COM_Path_f); // TODO: unused?
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
	AddGameDirectory(pPath); // TODO
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

/*
===========
COM_OpenFile

Filename never has a leading slash, but may contain directory walks
Returns a handle
It may actually be inside a pak file
If the requested file is inside a packfile, a new FILE * will be opened
into the file
===========
*/
FileHandle_t CFileSystem::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE_LOG("CFileSystem::Open(%s, %s, %s)", pFileName, pOptions, pathID);
	
	if(!pFileName || !pOptions)
		return nullptr;
	
	if(pOptions[0] == 'r')
		return FindFile(pFileName);
	
	return (FileHandle_t)fopen(pFileName, pOptions);
};

/*
============
COM_CloseFile

If it is a pak file handle, don't really close it
============
*/
void CFileSystem::Close(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Close(%p)", file);
	
	for(auto s = com_searchpaths; s; s = s->next)
		if(s->pack && s->pack->handle == file)
			return;
	
	if(file)
	{
		fclose((FILE*)file);
		file = nullptr;
	};
};

void CFileSystem::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	TRACE_LOG("CFileSystem::Seek(%p, %d, %d)", file, pos, seekType);
	fseek((FILE*)file, pos, seekType);
};

unsigned int CFileSystem::Tell(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Tell(%p)", file);
	return ftell((FILE*)file);
};

unsigned int CFileSystem::Size(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Size(%p)", file);
	return ::filelength((FILE*)file);
};

unsigned int CFileSystem::Size(const char *pFileName)
{
	TRACE_LOG("CFileSystem::Size(%s)", pFileName);
	
	auto pHandle{Open(pFileName, "r", "")};
	if(pHandle)
	{
		unsigned int nSize{Size(pHandle)};
		Close(pHandle);
		return nSize;
	};
	
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
	// TODO: returns int originally
	
#if defined(_WIN32) or defined(__linux__)
	struct stat buf;

	if(stat(pFileName, &buf) == -1)
		return -1;

	return buf.st_mtime;
#elif sun
	FILE *f{fopen(pFileName, "rb")};
	
	if(f)
	{
		fclose(f);
		return 1;
	};

	return -1;
#endif
};

void CFileSystem::FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime)
{
	TRACE_LOG("CFileSystem::FileTimeToString(%s, %d, %l", pStrip, maxCharsIncludingTerminator, fileTime);
};

bool CFileSystem::IsOk(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::IsOk(%p)", file);
	return false; // TODO
};

void CFileSystem::Flush(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Flush(%p)", file);
	fflush((FILE*)file);
};

bool CFileSystem::EndOfFile(FileHandle_t file)
{
	TRACE_LOG("CFileSystem::EndOfFile(%p)", file);
	return feof((FILE*)file);
};

int CFileSystem::Read(void *pOutput, int size, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Read(%p, %d, %p)", pOutput, size, file);
	return fread(pOutput, sizeof(char), size, (FILE*)file);
};

int CFileSystem::Write(void const *pInput, int size, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::Write(%p, %d, %p)", pInput, size, file);
	return fwrite(pInput, sizeof(char), size, (FILE*)file);
};

char *CFileSystem::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	TRACE_LOG("CFileSystem::ReadLine(%s, %d, %p)", pOutput, maxChars, file);
	return nullptr;
};

int CFileSystem::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	va_list argList{};
	char msg[1024]{};
	
	va_start(argList, pFormat);
	vsprintf(msg, pFormat, argList);
	va_end(argList);
	
	TRACE_LOG("CFileSystem::FPrintf(%p, %s)", file, msg);
	return fprintf((FILE*)file, msg);
};

void *CFileSystem::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	TRACE_LOG("CFileSystem::GetReadBuffer");
	return nullptr; // TODO
};

void CFileSystem::ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
	TRACE_LOG("CFileSystem::ReleaseReadBuffer(%d, %p)", file, readBuffer);
};

const char *CFileSystem::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	TRACE_LOG("CFileSystem::FindFirst(%s, %p, %d)", pWildCard, pHandle, pathID);
	return Sys_FindFirst(pathID, 0, 0); // TODO
};

const char *CFileSystem::FindNext(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindNext(%d)", handle);
	return Sys_FindNext(0, 0); // TODO
};

bool CFileSystem::FindIsDirectory(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindIsDirectory(%d)", handle);
	return false;
};

void CFileSystem::FindClose(FileFindHandle_t handle)
{
	TRACE_LOG("CFileSystem::FindClose(%d)", handle);
	Sys_FindClose(); // TODO
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

/*
===========
COM_FindFile

Finds the file in the search path.
===========
*/
FileHandle_t CFileSystem::FindFile(const char *filename)
{
	searchpath_t *search;
	char netpath[MAX_OSPATH];
	char cachepath[MAX_OSPATH];
	pack_t *pak;
	int i;
	FILE *pFile;
	int findtime, cachetime;

	//
	// search through the path, one element at a time
	//
	search = com_searchpaths;

	for(; search; search = search->next)
	{
		// is the element a pak file?
		if(search->pack)
		{
			printf("Search path %p is a pack! (%s)\n", search, search->filename);
			
			// look through all the pak file elements
			pak = search->pack;
			for(i = 0; i < pak->numfiles; i++)
				if(!strcmp(pak->files[i].name, filename))
				{
					// found it!
					//Sys_Printf("PackFile: %s : %s\n", pak->filename, filename); // TODO
					// open a new file on the pakfile
					pFile = fopen(pak->filename, "rb");
					if(pFile)
						fseek(pFile, pak->files[i].filepos, SEEK_SET); // TODO: pak->handle?
					return (FileHandle_t)pFile;
				};
		}
		else
		{
			// check a file in the directory tree

			sprintf(netpath, "%s/%s", search->filename, filename);

			findtime = GetFileTime(netpath);
			if(findtime == -1)
				continue;

			// see if the file needs to be updated in the cache
			if(!com_cachedir[0])
				strcpy(cachepath, netpath);
			else
			{
#if defined(_WIN32)
				if((strlen(netpath) < 2) || (netpath[1] != ':'))
					sprintf(cachepath, "%s%s", com_cachedir, netpath);
				else
					sprintf(cachepath, "%s%s", com_cachedir, netpath + 2);
#else
				sprintf(cachepath, "%s%s", com_cachedir, netpath);
#endif

				cachetime = GetFileTime(cachepath);

				//if(cachetime < findtime)
					//COM_CopyFile(netpath, cachepath); // TODO
				strcpy(netpath, cachepath);
			};

			//Sys_Printf("FindFile: %s\n", netpath); // TODO
			pFile = fopen(netpath, "rb"); // TODO: was i =
			return (FileHandle_t)pFile; // TODO
		};
	};

	//Sys_Printf("FindFile: can't find %s\n", filename); // TODO

	return nullptr;
};

/*
=================
COM_LoadPackFile

Takes an explicit (not game tree related) path to a pak file.

Loads the header and directory, adding the files at the beginning
of the list so they override previous pack files.
=================
*/
pack_t *CFileSystem::LoadPackFile(const char *packfile)
{
	dpackheader_t header;
	int i;
	packfile_t *newfiles;
	int numpackfiles;
	pack_t *pack;
	FILE *packhandle;
	dpackfile_t info[MAX_FILES_IN_PACK];

	packhandle = fopen(packfile, "rb");
	
	if(!packhandle)
	{
		//Con_Printf ("Couldn't open %s\n", packfile);
		printf("Couldn't open %s\n", packfile);
		return nullptr;
	};
	
	fread((void *)&header, 1, sizeof(header), packhandle);
	
	if(header.id[0] != 'P' || header.id[1] != 'A' || header.id[2] != 'C' || header.id[3] != 'K')
	{
		//Sys_Error("%s is not a packfile", packfile); // TODO
		return nullptr;
	};
	
	//header.dirofs = LittleLong(header.dirofs); // TODO
	//header.dirlen = LittleLong(header.dirlen); // TODO

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if(numpackfiles > MAX_FILES_IN_PACK)
	{
		//Sys_Error("%s has %i files", packfile, numpackfiles); // TODO
		return nullptr;
	};

	newfiles = (packfile_t*)malloc(numpackfiles * sizeof(packfile_t)); // TODO: was Hunk_AllocName(numpackfiles * sizeof(packfile_t), "packfile");

	fseek(packhandle, header.dirofs, SEEK_SET); 
	fread((void *)info, 1, header.dirlen, packhandle);

	// parse the directory
	for(i = 0; i < numpackfiles; i++)
	{
		strcpy(newfiles[i].name, info[i].name);
		//newfiles[i].filepos = LittleLong(info[i].filepos); // TODO
		//newfiles[i].filelen = LittleLong(info[i].filelen); // TODO
	};

	pack = (pack_t*)malloc(sizeof(pack_t)); // TODO: was Hunk_Alloc(sizeof(pack_t));
	memset(pack, 0, sizeof(pack));
	strcpy(pack->filename, packfile);
	pack->handle = (FileHandle_t)packhandle; // TODO
	pack->numfiles = numpackfiles;
	pack->files = newfiles;

	//Con_Printf("Added packfile %s (%i files)\n", packfile, numpackfiles); // TODO
	return pack;
};

/*
================
COM_AddGameDirectory

Sets com_gamedir, adds the directory to the head of the path,
then loads and adds pak1.pak pak2.pak ... 
================
*/
void CFileSystem::AddGameDirectory(const char *dir)
{
	int i;
	searchpath_t *search;
	pack_t *pak;
	char pakfile[MAX_OSPATH];

	//
	// add the directory to the search path
	//
	search = (searchpath_t*)malloc(sizeof(searchpath_t)); // TODO: was Hunk_Alloc(sizeof(searchpath_t));
	memset(search, 0, sizeof(search)); // TODO
	strcpy(search->filename, dir);
	search->pack = nullptr; // TODO
	search->next = com_searchpaths;
	com_searchpaths = search;

	//
	// add any pak files in the format pak0.pak pak1.pak, ...
	//
	for(i = 0;; i++)
	{
		sprintf(pakfile, "%s/pak%i.pak", dir, i);
		pak = LoadPackFile(pakfile);
		if(!pak)
			break;
		search = (searchpath_t*)malloc(sizeof(searchpath_t)); // TODO: was Hunk_Alloc(sizeof(searchpath_t));
		memset(search, 0, sizeof(search));
		search->pack = pak;
		search->next = com_searchpaths;
		com_searchpaths = search;
	}

	//
	// add the contents of the parms.txt file to the end of the command line
	//
};