/// @file

#include "FileSystem.hpp"

EXPOSE_SINGLE_INTERFACE(CFileSystem, IFileSystem, OGS_FILESYSTEM_INTERFACE_VERSION);

CFileSystem::CFileSystem() = default;
CFileSystem::~CFileSystem() = default;

void CFileSystem::Mount()
{
};

void CFileSystem::Unmount()
{
};