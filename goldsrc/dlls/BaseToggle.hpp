/// @file

#include "BaseDelay.hpp"

class CBaseToggle : public CBaseDelay
{
public:
	bool HandleKeyValue(KeyValueData *apKVData) override;
	
	int GetState() const {return mnState;}
	float GetDelay() const {return mfDelay;}
	
	void LinearMove();
	void LinearMoveDone();
	
	void AngularMove();
	void AngularMoveDone();
	
	bool IsLockedByMaster() const;
private:
	float mfDelay{0.0f};
	
	int mnState{-1};
};