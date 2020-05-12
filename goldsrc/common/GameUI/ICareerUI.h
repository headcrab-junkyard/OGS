/// @file

#pragma once

#include "tier1/interface.h"
#include "CareerDefs.h"

const char CAREERUI_INTERFACE_VERSION[] = "CareerUI001";

class ITaskVec;

enum CareerDifficultyType
{
	CAREER_DIFFICULTY_EASY,
	CAREER_DIFFICULTY_NORMAL,
	CAREER_DIFFICULTY_HARD,
	CAREER_DIFFICULTY_EXPERT,
	MAX_CAREER_DIFFICULTY
};

struct MapInfo
{
	char *name;
	bool defeated{false};
};

struct ICareerUI : public IBaseInterface
{
	virtual bool IsPlayingMatch() = 0;
	
	virtual ITaskVec *GetCurrentTaskVec() = 0;
	
	virtual bool PlayAsCT() = 0;
	
	virtual int GetReputationGained() = 0;
	virtual int GetNumMapsUnlocked() = 0;
	
	virtual bool DoesWinUnlockAll() = 0;
	
	virtual int GetRoundTimeLength() = 0;
	virtual int GetWinfastLength() = 0;
	
	virtual CareerDifficultyType GetDifficulty() const = 0;
	
	virtual int GetCurrentMapTriplet(MapInfo *apMaps) = 0;
	
	virtual void OnRoundEndMenuOpen(bool abDidWin) = 0;
	virtual void OnMatchEndMenuOpen(bool abDidWin) = 0;
	
	virtual void OnRoundEndMenuClose(bool abStillPlaying) = 0;
	virtual void OnMatchEndMenuClose(bool abStillPlaying) = 0;
};