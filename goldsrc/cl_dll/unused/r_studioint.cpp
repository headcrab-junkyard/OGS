/// @file

#include "r_studioint.h"

int StudioDrawModel(int flags)
{
	// TODO
	return 0;
};

int StudioDrawPlayer(int flags, struct entity_state_s *pPlayer)
{
	// TODO
	return 0;
};

r_studio_interface_t gStudioAPI =
{
	STUDIO_INTERFACE_VERSION,
	
	StudioDrawModel,
	StudioDrawPlayer
};

r_studio_interface_t *pStudioAPI = &gStudioAPI;