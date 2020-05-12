/// @file
/// @brief music manager interface

#pragma once

#include "tier1/interface.h"

const char MUSICMANAGER_INTERFACE_VERSION[] = "MusicManager001";

enum TrackType
{
	TRACKTYPE_INVALID = 1,

	TRACKTYPE_SPLASH,
	TRACKTYPE_ROUNDWIN,
	TRACKTYPE_ROUNDLOSE,
	TRACKTYPE_MATCHWIN,
	TRACKTYPE_MATCHLOSE,
	TRACKTYPE_CREDITS,

	TRACKTYPE_NUM
};

//-----------------------------------------------------------------------------
// Music manager interface (exported from gameui module)
// Purpose: Used to play the career ui events sounds (see TrackType enum)
//-----------------------------------------------------------------------------
struct IMusicManager : public IBaseInterface
{
	// param1 must be one of [2, 3, 4, 5, 6, 7]
	// param2 is "looping" if non-zero
	#define MUSICMANAGER_NOT_LOOPING 0
	#define MUSICMANAGER_LOOPING 1
	virtual void StartTrack(TrackType track, char looping) = 0;
	
	///
	virtual void StopTrack() = 0;
	
	///
	virtual void FadeOutTrack() = 0; // was FadeoutTrack
	
	///
	virtual bool IsPlayingTrack(TrackType track) = 0;

	/// Returns whether current track is number 1 ???
	//virtual bool Unknown1() = 0; // wtf?
};

/*
GSR

class IMusicManager : public IBaseInterface
{
public:
	virtual void StartTrack( TrackType track, bool looping ) = 0;
	virtual void StopTrack() = 0;
	virtual void FadeOutTrack() = 0;
	virtual bool IsPlayingTrack() = 0;
	virtual bool IsPlayingTrack( TrackType track ) = 0;
};
*/