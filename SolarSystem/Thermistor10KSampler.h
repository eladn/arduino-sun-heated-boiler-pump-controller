#ifndef THERMISTOR_10K_SAMPLER_H_
#define THERMISTOR_10K_SAMPLER_H_

#include "SensorSampler.h"

#define MAX_NR_OF_SAMPLES_BETWEEN_TWO_ERRONEOUS_TO_REPORT_FREQ_ERROR (SENSOR_READINGS_ARRAY_SIZE)
#define MIN_NR_OF_VALID_SAMPLES_TO_ABORT_FREQ_ERROR (3 * SENSOR_READINGS_ARRAY_SIZE)
#define VALID_TEMP_LOWER (-5)
#define VALID_TEMP_UPPER (140)
#define VALID_TEMP_MAX_ABS_DIFF (9)

enum SensorErrorState { NO_ERROR, ERROR_NOT_FREQ, ERROR_FREQ };

class Thermistor10KSampler : public SensorSampler {
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

#endif /* THERMISTOR_10K_SAMPLER_H_ */
