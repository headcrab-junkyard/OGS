/// @file
/// @brief cd audio interface

#pragma once

struct ICDAudio
{
	// TODO
	
	///
	virtual int Init() = 0;
	
	///
	virtual void Shutdown() = 0;
	
	///
	virtual void Frame() = 0;
	
	///
	//virtual void Play(byte track, qboolean looping) = 0;
	
	///
	//virtual void Stop() = 0;
	
	///
	virtual void Pause() = 0;
	
	///
	virtual void Resume() = 0;
};

extern ICDAudio *cdaudio; // gpCDAudio