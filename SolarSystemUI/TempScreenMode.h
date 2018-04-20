#ifndef TEMP_SCREEN_MODE_H_
#define TEMP_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class TempScreenMode : public UIScreenModeInterface<0, 0> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;  // Notice: We can extern it assuming there is only one.
	
public:
	TempScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface<0, 0>(),
		solarSystem(solarSystem)
	{}
	
	virtual void __loop() override {
		this->solarSystem->printStatusToLCD(this->lcd());
	}
	
};

#endif /* TEMP_SCREEN_MODE_H_ */
