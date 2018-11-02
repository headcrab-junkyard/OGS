/// @file

#include "r_studioint.h"

void SV_StudioSetupBones(struct model_s *pModel,
						float frame, int sequence,
						const vec3_t angles, const vec3_t origin,
						const byte *pController, const byte *pBlending,
						int nBone, const edict_t *pEdict)
{
	// TODO
};

sv_blending_interface_t gSvStudioAPI =
{
	SV_BLENDING_INTERFACE_VERSION,
	
	SV_StudioSetupBones
};