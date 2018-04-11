
/* 
    The program has been written by Elad Nachmias
    
    Based on schematics ideas by reuk.co.uk November 2013
    For information and the schematic circuit diagram for this
    solar water heating pump controller design, visit:
    http://www.reuk.co.uk/wordpress/arduino-solar-water-heating-pump-controller-design-and-code/
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <assert.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PANELSENSOR_PIN = A1;
const int TANKSENSOR_PIN = A0;
const int PUMP_RELAY_PIN = 10;

#define DELAY_MS_BETWEEN_SAMPLING 250

/* Relay module has opposite-logic. */
#define RELAY_ON(pin) digitalWrite(pin, LOW);
#define RELAY_OFF(pin) digitalWrite(pin, HIGH);


/* whether to constantly print temperatures to serial (for debugging) */
#define DEBUG_PRINT_TEMP_TO_SERIAL 0


/*
    Class for maintaining stable readings from a potentially unstable sensor.
    It saves the last `SENSOR_ARRAY_SIZE` readings from the sensor.
    Reading the current value by `getStableAvg()` returns an average of the last
    readings, while ignoring some outliers.
    Don't forget to make at lease `SENSOR_ARRAY_SIZE` samples before getting
    any value.
*/
#define SENSOR_ARRAY_SIZE 10
#define SENSOR_ARRAY_NR_OUTLIERS 3
class SensorSampler {
private:
  float readings[SENSOR_ARRAY_SIZE]; // contains the last `SENSOR_ARRAY_SIZE` sensor readings.
  int next_reading_idx; // next position to insert sample into readings array.
  float sum; // always contains the up-to-date sum of all readings.
  float avg; // always contains the up-to-date average of all readings.
  float stableAvg; // always contains the up-to-date stable average of all readings.
  
public:

  SensorSampler()
    : next_reading_idx(0), sum(0), avg(0), stableAvg(0) {
    for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) readings[i] = 0;
  }

  void writeSample(float sample) {
    this->sum -= this->readings[next_reading_idx];
    this->sum += sample;
    this->avg = (this->sum / SENSOR_ARRAY_SIZE);

    this->readings[this->next_reading_idx] = sample;

    // cyclic increment of index to next position in readings array.
    this->next_reading_idx++;
    if (this->next_reading_idx >= SENSOR_ARRAY_SIZE) this->next_reading_idx = 0;
    
    // we always keep the stable-avg up-to-date, so it won't have to be
    // re-calculated each time `getStableAvg()` is called.
    this->updateStableAvg();
  }

  inline float getAvg() {
    return this->avg;
  }

  inline float getStableAvg() {
    return this->stableAvg;
  }

private:
  
  // The following function updates the given `dist_from_avg` array, so that it  
  // would contain for each sample the distance between this sample and the average.
  // `dist_from_avg` should be allocated in size `SENSOR_ARRAY_SIZE` * sizeof(float).
  void calcReadingsDistFromAvg(float *dist_from_avg) {
    for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) {
      dist_from_avg[i] = abs(this->readings[i] - this->avg);
    }
  }
  
  // Puts into the given (and pre-allocated) `outliersIndexes` the indeces of/
  // the `nr_outliers` outliers (having the max distance from the average).
  void findOutliers(int* outliersIndexes, int nr_outliers) {
    assert(nr_outliers < SENSOR_ARRAY_SIZE);

    // We use a static array, so it won't be allocated on the stack each time
    // the function is called. Assuming the calls to this function are not concurrent.
    static float dist_from_avg[SENSOR_ARRAY_SIZE];
    this->calcReadingsDistFromAvg(dist_from_avg);

    for (int cur_outlier = 0; cur_outlier < nr_outliers; ++cur_outlier) {
      float cur_max_dist = 0;
      int cur_max_dist_idx = 0;
      for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) {
        if (dist_from_avg[i] >= cur_max_dist) {
          cur_max_dist = dist_from_avg[i];
          cur_max_dist_idx = i;
        }
      }
      outliersIndexes[cur_outlier] = cur_max_dist_idx;
      dist_from_avg[cur_max_dist_idx] = -1;
    }
  }
  
  void updateStableAvg() {
    // Find `SENSOR_ARRAY_NR_OUTLIERS` outliers.
    static int outliersIndexes[SENSOR_ARRAY_NR_OUTLIERS];
    findOutliers(outliersIndexes, SENSOR_ARRAY_NR_OUTLIERS);

    // Assign `true` at place #i in the mask if-and-only-if #i is NOT an outlier,
    // so that we will know to discard the outliers later when calculating the average.
    static bool readingsMask[SENSOR_ARRAY_SIZE];
    for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) readingsMask[i] = true;
    for (int i = 0; i < SENSOR_ARRAY_NR_OUTLIERS; ++i) {
      assert(outliersIndexes[i] >= 0 && outliersIndexes[i] < SENSOR_ARRAY_SIZE);
      readingsMask[outliersIndexes[i]] = false;
    }

    // Calculate the sum of the values without the outliers.
    float localSum = 0;
    for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) {
      if(readingsMask[i]) localSum += this->readings[i];
    }

    // Update the stable average.
    this->stableAvg = (localSum / (SENSOR_ARRAY_SIZE - SENSOR_ARRAY_NR_OUTLIERS));
  }
};

#define MAX_NR_OF_SAMPLES_BETWEEN_TWO_ERRONEOUS_TO_REPORT_FREQ_ERROR (SENSOR_ARRAY_SIZE)
#define MIN_NR_OF_VALID_SAMPLES_TO_ABORT_FREQ_ERROR (3 * SENSOR_ARRAY_SIZE)
#define VALID_TEMP_LOWER (-5)
#define VALID_TEMP_UPPER (140)
#define VALID_TEMP_MAX_ABS_DIFF (9)
enum SensorErrorState { NO_ERROR, ERROR_NOT_FREQ, ERROR_FREQ };
class Thermistor10KSampler : SensorSampler {
private:
  const int thermistorPin;
  unsigned int nr_samples;
  struct {
    enum SensorErrorState errorState;
    unsigned int lastErroneousSampleNum;
  } sensorError;
    
public:

  Thermistor10KSampler(int thermistorPin)
  : SensorSampler(), 
    thermistorPin(thermistorPin), 
    nr_samples(0), 
    sensorError({ 
      .errorState = SensorErrorState::NO_ERROR, 
      .lastErroneousSampleNum = 0 
    })
  {}

  void init() {
    pinMode(this->thermistorPin, INPUT);
  }
  
  void sample() {
    static const float R1 = 10000;
    static const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
    
    int Vo;
    float logR2, R2, celsiusTemp;
    
    Vo = analogRead(this->thermistorPin);
    
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    celsiusTemp = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)) - 273.15;
    
    // increment the number of samples. will set back to zero after max-int.
    this->nr_samples++;
    
    if (isValidTemperature(celsiusTemp)) {
      this->markCurrentSampleValid();
    } else {
      this->markCurrentSampleErroneous();
    }
    
    this->writeSample(celsiusTemp);
  }
  
  inline float getTemperature() {
    return this->getStableAvg();
  }
  
  inline bool isErrorOccurred() {
    return this->sensorError.errorState == SensorErrorState::ERROR_FREQ;
  }
  
private:
  
  inline bool isValidTemperature(float temperature) {
    return (temperature >= VALID_TEMP_LOWER
        && temperature <= VALID_TEMP_UPPER
        && abs(this->getStableAvg() - temperature) <= VALID_TEMP_MAX_ABS_DIFF);
  }
  
  inline void markCurrentSampleErroneous() {
    if (this->sensorError.errorState == SensorErrorState::NO_ERROR) {
      this->sensorError.errorState = SensorErrorState::ERROR_NOT_FREQ;
    } else if (this->sensorError.errorState == SensorErrorState::ERROR_NOT_FREQ
      && (this->nr_samples - this->sensorError.lastErroneousSampleNum) <= MAX_NR_OF_SAMPLES_BETWEEN_TWO_ERRONEOUS_TO_REPORT_FREQ_ERROR) {
      this->sensorError.errorState = SensorErrorState::ERROR_FREQ;
    }
    // Notice: If the state already was `ERROR_FREQ`, the state stays as it has been before without any additional conditions.
    
    this->sensorError.lastErroneousSampleNum = this->nr_samples;
  }
  
  inline void markCurrentSampleValid() {
      if (this->sensorError.errorState == SensorErrorState::ERROR_FREQ
        && (this->nr_samples - this->sensorError.lastErroneousSampleNum) > MIN_NR_OF_VALID_SAMPLES_TO_ABORT_FREQ_ERROR) {
          this->sensorError.errorState = SensorErrorState::ERROR_NOT_FREQ;
      }
      if (this->sensorError.errorState == SensorErrorState::ERROR_NOT_FREQ
        && (this->nr_samples - this->sensorError.lastErroneousSampleNum) > MAX_NR_OF_SAMPLES_BETWEEN_TWO_ERRONEOUS_TO_REPORT_FREQ_ERROR) {
          this->sensorError.errorState = SensorErrorState::NO_ERROR;
      }
  }
};


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
    for (int i = 0; i < SENSOR_ARRAY_SIZE; ++i) {
      solarPanelSampler.sample();
      hotWaterTankSampler.sample();
      delay(DELAY_MS_BETWEEN_SAMPLING);
    }
  }
  
  void updateStatus() {
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

SolarSystem solarSystem(PANELSENSOR_PIN, TANKSENSOR_PIN, PUMP_RELAY_PIN);


/* Main Arduino setup() method */
void setup() {
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  Serial.begin(9600);
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  lcd.init();  //initialize the LCD
  lcd.backlight();  //open the back-light
  
  solarSystem.init();
}

/* Main Arduino loop() method */
void loop() {
  solarSystem.updateStatus();
  
  solarSystem.printStatusToLCD(lcd);
  #if DEBUG_PRINT_TEMP_TO_SERIAL
  solarSystem.printStatusToSerial();
  #endif /* DEBUG_PRINT_TEMP_TO_SERIAL */
  
  delay(DELAY_MS_BETWEEN_SAMPLING);
}
