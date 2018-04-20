
/* 
    The program has been written by Elad Nachmias
    
    Based on schematics ideas by reuk.co.uk November 2013
    For information and the schematic circuit diagram for this
    solar water heating pump controller design, visit:
    http://www.reuk.co.uk/wordpress/arduino-solar-water-heating-pump-controller-design-and-code/
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "SolarSystemSettings.h"

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

#include "SolarSystem/SolarSystem.h"
SolarSystem solarSystem(PANEL_SENSOR_PIN, TANK_SENSOR_PIN, PUMP_RELAY_PIN);

#if USE_SOLAR_SYSTEM_UI
#include "SolarSystemUI/SolarSystemUI.h"
UILcdInterfaceImpl<LiquidCrystal_I2C> lcdInterface(&lcd);
SolarSystemUI solarSystemUI(&solarSystem, &lcdInterface, MODE_BUTTON_PIN, 
							UPPER_BUTTON_PIN, LOWER_BUTTON_PIN, SET_BUTTON_PIN);
#endif /* USE_SOLAR_SYSTEM_UI */


/* Main Arduino setup() method */
void setup() {
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  Serial.begin(9600);
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  lcd.init();  // initialize the LCD
  lcd.backlight();  // open the back-light
  
  solarSystem.init();
  
  #if USE_SOLAR_SYSTEM_UI
  solarSystemUI.init();
  #endif /* USE_SOLAR_SYSTEM_UI */
}

/* Main Arduino loop() method */
void loop() {
  solarSystem.loop();
  
  #if USE_SOLAR_SYSTEM_UI
  solarSystemUI.loop();
  #else
  // Simple printing without the `SolarSystemUI`:
  solarSystem.printStatusToLCD(lcd);
  #endif /* USE_SOLAR_SYSTEM_UI */
  
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  solarSystem.printStatusToSerial();
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  delay(MAIN_LOOP_DELAY_MS);
}
