/// @file

#include "quakedef.h"
#include "icdaudio.h"

// TODO: bgmvolume

void CD_Command_f()
{
	//if(COM_CheckParm("-nocdaudio") || COM_CheckParm("-nosound")) // TODO: <-- ADD THIS (Bugfix; Not present in GS)
		//return;
	
	// ...
	
	if(!stricmp(command, "on"))
	{
		g_CDAudio.m_bEnabled = true;
		return;
	};
	
	// ...
};

class CCDAudio final : public ICDAudio
{
public:
	CCDAudio();
	~CCDAudio();
	
	void Frame() override;
	
	void Pause() override;
	void Resume() override;
	
	void MP3_Init();
	void MP3_InitStream();
	
	float m_flVolume{0.0f}; // private?
	bool m_bEnabled{false};
};

static CCDAudio g_CDAudio;
ICDAudio *cdaudio = &g_CDAudio;

CCDAudio::CCDAudio() = default;
CCDAudio::~CCDAudio() = default;

void CCDAudio::Frame()
{
	// This is set to TRUE when "cd on" went to the console
	if(!m_bEnabled)
		return;

	if(m_flVolume != bgmvolume.value)
	{
		if(m_flVolume)
		{
			m_flVolume = 0.0f;
			Pause();
		}
		else
		{
			m_flVolume = 1.0f;
			Resume();
		};
		
		Cvar_DirectSet(&bgmvolume, va("%f", m_flVolume)); // cvar_t bgmvolume is not registered, you should never call Cvar_DirectSet with an unregistered cvar!
	};
	
	// ...
};

void CCDAudio::MP3_Init()
{
	// TODO
};

void CCDAudio::MP3_InitStream(int trackNum, bool looping)
{
	// TODO
};