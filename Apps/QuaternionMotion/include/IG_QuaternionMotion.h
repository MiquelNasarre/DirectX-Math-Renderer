#pragma once
#include "iGManager.h"

class IG_QuaternionMotion : public iGManager
{
private:
	void saveLightState(int id);
	void loadLightState(int id);
	void doLightEditor(int& id);

public:
	IG_QuaternionMotion() : iGManager() {}
	void render() override;
};
