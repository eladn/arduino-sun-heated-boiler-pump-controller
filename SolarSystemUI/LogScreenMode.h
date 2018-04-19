#ifndef LOG_SCREEN_MODE_H_
#define LOG_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class LogScreenMode : public UIScreenModeInterface<void*> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;
	
public:
	LogScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface(),
		solarSystem(solarSystem)
	{}
	
	// TODO: implement!
	
};

#endif /* LOG_SCREEN_MODE_H_ */
