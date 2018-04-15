#ifndef SENSOR_SAMPLER_H_
#define SENSOR_SAMPLER_H_

#include <assert.h>

/*
    Class for maintaining stable readings from a potentially unstable sensor.
    It saves the last `SENSOR_READINGS_ARRAY_SIZE` readings from the sensor.
    Reading the current value by `getStableAvg()` returns an average of the last
    readings, while ignoring some outliers.
    Don't forget to make at lease `SENSOR_READINGS_ARRAY_SIZE` samples before getting
    any value.
*/
#define SENSOR_READINGS_ARRAY_SIZE 10
#define SENSOR_ARRAY_NR_OUTLIERS 3
class SensorSampler {
private:
  float readings[SENSOR_READINGS_ARRAY_SIZE]; // contains the last `SENSOR_READINGS_ARRAY_SIZE` sensor readings.
  int next_reading_idx; // next position to insert sample into readings array.
  float sum; // always contains the up-to-date sum of all readings.
  float avg; // always contains the up-to-date average of all readings.
  float stableAvg; // always contains the up-to-date stable average of all readings.
  
public:

  SensorSampler()
    : next_reading_idx(0), sum(0), avg(0), stableAvg(0) {
    for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) readings[i] = 0;
  }

  void writeSample(float sample) {
    this->sum -= this->readings[next_reading_idx];
    this->sum += sample;
    this->avg = (this->sum / SENSOR_READINGS_ARRAY_SIZE);

    this->readings[this->next_reading_idx] = sample;

    // cyclic increment of index to next position in readings array.
    this->next_reading_idx++;
    if (this->next_reading_idx >= SENSOR_READINGS_ARRAY_SIZE) this->next_reading_idx = 0;
    
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
  // `dist_from_avg` should be allocated in size `SENSOR_READINGS_ARRAY_SIZE` * sizeof(float).
  void calcReadingsDistFromAvg(float *dist_from_avg) {
    for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) {
      dist_from_avg[i] = abs(this->readings[i] - this->avg);
    }
  }
  
  // Puts into the given (and pre-allocated) `outliersIndexes` the indeces of/
  // the `nr_outliers` outliers (having the max distance from the average).
  void findOutliers(int* outliersIndexes, int nr_outliers) {
    assert(nr_outliers < SENSOR_READINGS_ARRAY_SIZE);

    // We use a static array, so it won't be allocated on the stack each time
    // the function is called. Assuming the calls to this function are not concurrent.
    static float dist_from_avg[SENSOR_READINGS_ARRAY_SIZE];
    this->calcReadingsDistFromAvg(dist_from_avg);

    for (int cur_outlier = 0; cur_outlier < nr_outliers; ++cur_outlier) {
      float cur_max_dist = 0;
      int cur_max_dist_idx = 0;
      for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) {
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
    static bool readingsMask[SENSOR_READINGS_ARRAY_SIZE];
    for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) readingsMask[i] = true;
    for (int i = 0; i < SENSOR_ARRAY_NR_OUTLIERS; ++i) {
      assert(outliersIndexes[i] >= 0 && outliersIndexes[i] < SENSOR_READINGS_ARRAY_SIZE);
      readingsMask[outliersIndexes[i]] = false;
    }

    // Calculate the sum of the values without the outliers.
    float localSum = 0;
    for (int i = 0; i < SENSOR_READINGS_ARRAY_SIZE; ++i) {
      if(readingsMask[i]) localSum += this->readings[i];
    }

    // Update the stable average.
    this->stableAvg = (localSum / (SENSOR_READINGS_ARRAY_SIZE - SENSOR_ARRAY_NR_OUTLIERS));
  }
};

#endif /* SENSOR_SAMPLER_H_ */
