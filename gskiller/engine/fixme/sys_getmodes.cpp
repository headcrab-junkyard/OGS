/// @file

// BP: I don't think we need this crap...

struct IVideoMode
{
};

class CVideoMode_OpenGL final : public IVideoMode
{
public:
	CVideoMode_OpenGL();
	~CVideoMode_OpenGL();
	
	void Init(void *pvInstance);
};

class CVideoMode_Common final : public IVideoMode
{
public:
	CVideoMode_Common();
	~CVideoMode_Common();
};