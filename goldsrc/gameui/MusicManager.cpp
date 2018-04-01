/// @file

#include "MusicManager.hpp"

EXPOSE_SINGLE_INTERFACE(CMusicManager, IMusicManager, MUSICMANAGER_INTERFACE_VERSION);

CMusicManager::CMusicManager() = default;
CMusicManager::~CMusicManager() = default;

void CMusicManager::StartTrack(TrackType track, bool looping)
{
};

void CMusicManager::StopTrack()
{
};

void CMusicManager::FadeOutTrack()
{
};

bool CMusicManager::IsPlayingTrack(TrackType track)
{
	return false;
};

bool CMusicManager::IsPlayingTrack()
{
	return false;
};