#ifndef TEMP_DIFF_SETTING_SCREEN_MODE_H_
#define TEMP_DIFF_SETTING_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class TempDiffSettingScreenMode : public UIScreenModeInterface<void*> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;
	
public:
	TempDiffSettingScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface(),
		solarSystem(solarSystem)
	{}
	
	// TODO: implement!
	
};

#endif /* TEMP_DIFF_SETTING_SCREEN_MODE_H_ */
