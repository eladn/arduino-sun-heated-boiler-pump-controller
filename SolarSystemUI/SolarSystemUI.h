#ifndef SOLAR_SYSTEM_UI_H_
#define SOLAR_SYSTEM_UI_H_

#include "../SolarSystem/SolarSystem.h"
#include "../UI/UIHandler.h"
#include "TempScreenMode.h"
#include "LogScreenMode.h"
#include "TempDiffSettingScreenMode.h"

#define MODE_BUTTON_PIN 0 /* TODO: change! */
#define UPPER_BUTTON_PIN 0 /* TODO: change! */
#define UPPER_BUTTON_IDX 1
#define LOWER_BUTTON_PIN 0 /* TODO: change! */ 
#define LOWER_BUTTON_IDX 2
#define NEXT_BUTTON_PIN 0  /* TODO: change! */
#define NEXT_BUTTON_IDX 3

class SolarSystemUI {
private:
	UIHandler uiHandler;
	TempScreenMode tempScreenMode;
	LogScreenMode logScreenMode;
	TempDiffSettingScreenMode tempDiffSettingScreenMode;

public:
	SolarSystemUI(SolarSystem* solarSystem, UILcdInterface *lcd)
		: uiHandler(MODE_BUTTON_PIN, lcd),
		tempScreenMode(solarSystem),
		logScreenMode(solarSystem),
		tempDiffSettingScreenMode(solarSystem)
	{
		uiHandler.setButton(UPPER_BUTTON_PIN, UPPER_BUTTON_IDX);
		uiHandler.setButton(LOWER_BUTTON_PIN, LOWER_BUTTON_IDX);
		uiHandler.setButton(NEXT_BUTTON_PIN, NEXT_BUTTON_IDX);
		
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
