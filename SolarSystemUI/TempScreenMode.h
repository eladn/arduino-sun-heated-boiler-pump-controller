#ifndef TEMP_SCREEN_MODE_H_
#define TEMP_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class TempScreenMode : public UIScreenModeInterface<void*> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;
	
public:
	TempScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface(),
		solarSystem(solarSystem)
	{}
	
	// TODO: implement!
	
};

#endif /* TEMP_SCREEN_MODE_H_ */
