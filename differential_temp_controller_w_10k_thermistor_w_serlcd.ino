
/* 
    The program has been written by Elad Nachmias
    
    Based on schematics ideas by reuk.co.uk November 2013
    For information and the schematic circuit diagram for this
    solar water heating pump controller design, visit:
    http://www.reuk.co.uk/wordpress/arduino-solar-water-heating-pump-controller-design-and-code/
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PANELSENSOR_PIN = A1;
const int TANKSENSOR_PIN = A0;
const int PUMP_RELAY_PIN = 10;

#define DELAY_MS_BETWEEN_SAMPLING 250

/* whether to constantly print temperatures to serial (for debugging) */
#define DEBUG_PRINT_TEMP_TO_SERIAL 0

#include "SolarSystem.h"


SolarSystem solarSystem(PANELSENSOR_PIN, TANKSENSOR_PIN, PUMP_RELAY_PIN);


/* Main Arduino setup() method */
void setup() {
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  Serial.begin(9600);
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  lcd.init();  // initialize the LCD
  lcd.backlight();  // open the back-light
  
  solarSystem.init();
}

/* Main Arduino loop() method */
void loop() {
  solarSystem.loop();
  
  solarSystem.printStatusToLCD(lcd);
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  solarSystem.printStatusToSerial();
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  delay(DELAY_MS_BETWEEN_SAMPLING);
}
