/// @file

#pragma once

#include "GameUI/IMusicManager.h"

class CMusicManager final : public IMusicManager
{
public:
	CMusicManager();
	~CMusicManager();
	
	void StartTrack(TrackType track, bool looping) override;
	void StopTrack() override;

	void FadeOutTrack() override;

	bool IsPlayingTrack(TrackType track) override;
	bool IsPlayingTrack() override;
};