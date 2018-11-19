/// @file
/// @brief run game engine interface

#pragma once

#include "interface.h"

const char RUNGAMEENGINE_INTERFACE_VERSION[] = "RunGameEngine006";

//-----------------------------------------------------------------------------
// Purpose: Interface to running the game engine
//-----------------------------------------------------------------------------
struct IRunGameEngine : public IBaseInterface
{
	// Returns true if the engine is running, false otherwise.
	virtual bool IsRunning() = 0;

	// Adds text to the engine command buffer. Only works if IsRunning()
	// returns true on success, false on failure
	virtual bool AddTextCommand( const char *text ) = 0;

	// runs the engine with the specified command line parameters.  Only works if !IsRunning()
	// returns true on success, false on failure
	virtual bool RunEngine( const char *gameDir , const char *commandLineParams ) = 0;

	// returns true if the player is currently connected to a game server
	virtual bool IsInGame() = 0;

	// gets information about the server the engine is currently connected to
	// returns true on success, false on failure
	virtual bool GetGameInfo( char *infoBuffer , int bufferSize ) = 0;

	// tells the engine our userID
	virtual void SetTrackerUserID( int trackerID , const char *trackerName ) = 0;

	// this next section could probably moved to another interface
	// iterates users
	// returns the number of user
	virtual int GetPlayerCount() = 0;

	// returns a playerID for a player
	// playerIndex is in the range [0, GetPlayerCount)
	virtual unsigned int GetPlayerFriendsID( int playerIndex ) = 0;

	// gets the in-game name of another user, returns NULL if that user doesn't exists
	virtual const char *GetPlayerName( int friendsID ) = 0;

	// gets the friends name of a player
	virtual const char *GetPlayerFriendsName( int friendsID ) = 0;

	// returns the engine build number and mod version string for server versioning
	virtual unsigned int GetEngineBuildNumber() = 0;
	virtual const char *GetProductVersionString() = 0;
	virtual unsigned int GetPlayerUserID( int friendsID ) = 0;
};

extern IRunGameEngine* g_pIRunGameEngine;