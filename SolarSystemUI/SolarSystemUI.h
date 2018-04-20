#ifndef SOLAR_SYSTEM_UI_H_
#define SOLAR_SYSTEM_UI_H_

#define UPPER_BUTTON_IDX 1
#define LOWER_BUTTON_IDX 2
#define SET_BUTTON_IDX 3

#include "../SolarSystem/SolarSystem.h"
#include "../UI/UIHandler.h"
#include "TempScreenMode.h"
#include "LogScreenMode.h"
#include "TempDiffSettingScreenMode.h"

class SolarSystemUI {
private:
	UIHandler uiHandler;
	TempScreenMode tempScreenMode;
	LogScreenMode logScreenMode;
	TempDiffSettingScreenMode tempDiffSettingScreenMode;

public:
	SolarSystemUI(SolarSystem* solarSystem, UILcdInterface *lcd, int modeButtonPin, 
				int upperButtonPin, int lowerButtonPin, int setButtonPin)
		: uiHandler(modeButtonPin, lcd),
		tempScreenMode(solarSystem),
		logScreenMode(solarSystem),
		tempDiffSettingScreenMode(solarSystem)
	{
		uiHandler.setButton(upperButtonPin, UPPER_BUTTON_IDX);
		uiHandler.setButton(lowerButtonPin, LOWER_BUTTON_IDX);
		uiHandler.setButton(setButtonPin, SET_BUTTON_IDX);
		
		uiHandler.addScreenMode(&this->tempScreenMode);
		uiHandler.addScreenMode(&this->logScreenMode);
		uiHandler.addScreenMode(&this->tempDiffSettingScreenMode);
	}
	
	inline void init() {
		uiHandler.init();
	}
	
	inline void loop() {
		uiHandler.loop();
	}
};

#endif /* SOLAR_SYSTEM_UI_H_ */
