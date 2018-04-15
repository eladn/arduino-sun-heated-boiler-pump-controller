#ifndef SOLAR_SYSTEM_H_
#define SOLAR_SYSTEM_H_

#include "Thermistor10KSampler.h"
#include "Pump.h"

class SolarSystem {
private:
  static constexpr float tempDiffON = 10;
  static constexpr float tempDiffOFF = 3;

  // Objects to handle the sample readings of the two sensors.
  Thermistor10KSampler solarPanelSampler;
  Thermistor10KSampler hotWaterTankSampler;
  Pump pump;

public:
  SolarSystem(int panelSensorPin, int tankSensorPin, int pumpRelayPin)
    : solarPanelSampler(panelSensorPin),
    hotWaterTankSampler(tankSensorPin),
    pump(pumpRelayPin)
    {}
  
  void init() {
    pump.init();
    solarPanelSampler.init();
    hotWaterTankSampler.init();
    // fill the sensors data with initial values.
    for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) {
      solarPanelSampler.sample();
      hotWaterTankSampler.sample();
      delay(DELAY_MS_BETWEEN_SAMPLING);
    }
  }
  
  void loop() {
    this->solarPanelSampler.sample();
    this->hotWaterTankSampler.sample();
    
    if(!this->pump.isOn() && this->shouldTurnPumpOn()) {
      this->pump.turnOn();
    } else if (this->pump.isOn() && this->shouldTurnPumpOff()) {
      this->pump.turnOff();
    }
  }
  
  inline float temperaturesDiff() {
    return this->solarPanelSampler.getTemperature() - this->hotWaterTankSampler.getTemperature();
  }
  
  inline bool shouldTurnPumpOn() {
    return this->temperaturesDiff() >= SolarSystem::tempDiffON;
  }
  
  inline bool shouldTurnPumpOff() {
    return this->temperaturesDiff() <= SolarSystem::tempDiffOFF;
  }
  
  // The following function is used inside the main loop() to display the
  // temperatures and the pump state on the LCD. We use generics (`template`)
  // to support any kind of lcd that has `setCursor` and `print` methods.
  template <typename LCD_TYPE>
  void printStatusToLCD(LCD_TYPE& lcd) {
    // first line.
    lcd.setCursor(0,0);
    if (this->solarPanelSampler.isErrorOccurred()) lcd.print("*"); else lcd.print(" "); // error indicator
    lcd.print("Panel ");
    lcd.print(this->solarPanelSampler.getTemperature(), 1); // 4-5 chars
    lcd.print(" ");
    
    // second line.
    lcd.setCursor(0,1);
    if (this->hotWaterTankSampler.isErrorOccurred()) lcd.print("*"); else lcd.print(" "); // error indicator
    lcd.print("Tank  ");
    lcd.print(this->hotWaterTankSampler.getTemperature(), 1); // 4-5 chars
    lcd.print(" ");
    
    // print whether the pump is currently turned-on, on the top-right corner.
    lcd.setCursor(13,0);
    if (this->pump.isOn()) {
      lcd.print("ON");
    } else {
      lcd.print("  ");
    }
  }

#if DEBUG_PRINT_TEMP_TO_SERIAL
  void printStatusToSerial() {
    Serial.print("Panel Temp: ");
    Serial.print(solarPanelSampler.getTemperature());
    Serial.print("   Tank Temp: ");
    Serial.print(hotWaterTankSampler.getTemperature());
    Serial.print("   Pump: ");
    if (this->pump.isOn()) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }
#endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
};

#endif /* SOLAR_SYSTEM_H_ */
