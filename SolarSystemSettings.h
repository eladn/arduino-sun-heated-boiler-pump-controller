#ifndef SOLAR_SYSTEM_PINS_SETTINGS_H_
#define SOLAR_SYSTEM_PINS_SETTINGS_H_

#define MAIN_LOOP_DELAY_MS 250

/* whether to constantly print temperatures to serial (for debugging) */
#define DEBUG_PRINT_TEMP_TO_SERIAL 0

/* whether to use the advanced UI mechanism */
#define USE_SOLAR_SYSTEM_UI 1

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define PANEL_SENSOR_PIN A1
#define TANK_SENSOR_PIN A0
#define PUMP_RELAY_PIN 10
#define MODE_BUTTON_PIN 1 /* TODO: change! */
#define UPPER_BUTTON_PIN 2 /* TODO: change! */
#define LOWER_BUTTON_PIN 3 /* TODO: change! */ 
#define SET_BUTTON_PIN 4  /* TODO: change! */

#endif /* SOLAR_SYSTEM_PINS_SETTINGS_H_ */
