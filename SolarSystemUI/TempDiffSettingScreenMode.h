#ifndef TEMP_DIFF_SETTING_SCREEN_MODE_H_
#define TEMP_DIFF_SETTING_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class TempDiffSettingScreenMode : public UIScreenModeInterface<3, 1> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;  // Notice: We can extern it assuming there is only one.
	
public:
	TempDiffSettingScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface<3, 1>(),
		solarSystem(solarSystem)
	{}
	
	// TODO: implement!
	
};

#endif /* TEMP_DIFF_SETTING_SCREEN_MODE_H_ */
