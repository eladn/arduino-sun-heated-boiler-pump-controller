#ifndef PUMP_H_
#define PUMP_H_

#include "Relay.h"

class Pump {
private:
  const int pumpRelayPin;
  bool isPumpON;
public:
  Pump(int pumpRelayPin) : pumpRelayPin(pumpRelayPin), isPumpON(false) {}
  void init() {
    pinMode(this->pumpRelayPin, OUTPUT);
    this->turnOff();
  }
  inline void turnOn() {
    this->isPumpON = true;
    RELAY_ON(this->pumpRelayPin);
  }
  inline void turnOff() {
    this->isPumpON = false;
    RELAY_OFF(this->pumpRelayPin);
  }
  inline bool isOn() {
    return this->isPumpON;
  }
};

#endif /* PUMP_H_ */
