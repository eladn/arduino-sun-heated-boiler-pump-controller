#ifndef LOG_SCREEN_MODE_H_
#define LOG_SCREEN_MODE_H_

#include "../UI/UIScreenModeInterface.h"
#include "../SolarSystem/SolarSystem.h"

class LogScreenMode : public UIScreenModeInterface<2, 1> /* TODO: define <ButtonEventsArgType> template */ {
private:
	SolarSystem* solarSystem;  // Notice: We can extern it assuming there is only one.
	
public:
	LogScreenMode(SolarSystem* solarSystem)
		: UIScreenModeInterface<2, 1>(),
		solarSystem(solarSystem)
	{}
	
	// TODO: implement!
	
};

#endif /* LOG_SCREEN_MODE_H_ */
