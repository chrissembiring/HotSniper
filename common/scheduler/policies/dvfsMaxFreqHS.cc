#include "dvfsMaxFreqHS.h"
#include <iomanip>
#include <iostream>

using namespace std;

DVFSMaxFreqHS::DVFSMaxFreqHS(const PerformanceCounters *performanceCounters,
                         int coreRows, int coreColumns, int maxFrequency)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency) {}

std::vector<int>
DVFSMaxFreqHS::getFrequencies(const std::vector<int> &oldFrequencies,
                            const std::vector<bool> &activeCores) {
  std::vector<int> frequencies(coreRows * coreColumns);

  for (unsigned int coreCounter = 0; coreCounter < coreRows * coreColumns;
       coreCounter++) {
    if (activeCores.at(coreCounter)) {
      float power = performanceCounters->getPowerOfCore(coreCounter);
      float temperature =
          performanceCounters->getTemperatureOfCore(coreCounter);
      int frequency = oldFrequencies.at(coreCounter);
      float utilization =
          performanceCounters->getUtilizationOfCore(coreCounter);

      cout << "[Scheduler][DVFS_MAX_FREQ_HS]: Core " << setw(2) << coreCounter
           << ":";
      cout << " P=" << fixed << setprecision(3) << power << " W";
      cout << " f=" << frequency << " MHz";
      cout << " T=" << fixed << setprecision(1) << temperature << " °C";
      cout << " utilization=" << fixed << setprecision(3) << utilization
           << endl;
    }
    frequencies.at(coreCounter) = maxFrequency;
  }

  return frequencies;
}

DVFSMinMaxFreqHS::DVFSMinMaxFreqHS(const PerformanceCounters *performanceCounters,
                               int coreRows, int coreColumns, int maxFrequency,
                               int minFrequency)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency),
      minFrequency(minFrequency) {}

std::vector<int>
DVFSMinMaxFreqHS::getFrequencies(const std::vector<int> &oldFrequencies,
                               const std::vector<bool> &activeCores) {
  std::vector<int> frequencies(coreRows * coreColumns);

  for (unsigned int coreCounter = 0; coreCounter < coreRows * coreColumns;
       coreCounter++) {
    if (activeCores.at(coreCounter)) {
      float power = performanceCounters->getPowerOfCore(coreCounter);
      float temperature =
          performanceCounters->getTemperatureOfCore(coreCounter);
      int frequency = oldFrequencies.at(coreCounter);
      float utilization =
          performanceCounters->getUtilizationOfCore(coreCounter);

      cout << "[Scheduler][DVFS_MINMAX_FREQ_HS]: Core " << setw(2) << coreCounter
           << ":";
      cout << " P=" << fixed << setprecision(3) << power << " W";
      cout << " f=" << frequency << " MHz";
      cout << " T=" << fixed << setprecision(1) << temperature << " °C";
      cout << " utilization=" << fixed << setprecision(3) << utilization
           << endl;
    }
    if (coreCounter % 2 == 0)
      frequencies.at(coreCounter) = maxFrequency;
    else
      frequencies.at(coreCounter) = minFrequency;
  }

  return frequencies;
}

DVFSchedFreqHS::DVFSchedFreqHS(const PerformanceCounters *performanceCounters,
                           int coreRows, int coreColumns, int maxFrequency,
                           int minFrequency, float dtmCriticalTemperature,
                           float dtmRecoveredTemperature, PELT *pelt)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency),
      minFrequency(minFrequency),
      dtmCriticalTemperature(dtmCriticalTemperature),
      dtmRecoveredTemperature(dtmRecoveredTemperature), pelt(pelt) {

        FILE * file = fopen("model_file", "wb");
        if (file != NULL) {
            fread(&model, sizeof(RC_model_t), 1, file);
            fclose(file);
        }
      }

std::vector<int>
DVFSchedFreqHS::getFrequencies(const std::vector<int> &oldFrequencies,
                             const std::vector<bool> &activeCores) {
  std::vector<int> frequencies(coreRows * coreColumns);

  for (unsigned int coreCounter = 0; coreCounter < coreRows * coreColumns;
       coreCounter++) {

    bool tlt = throttle();
    if (activeCores.at(coreCounter)) {
      float power = performanceCounters->getPowerOfCore(coreCounter);
      float temperature =
          performanceCounters->getTemperatureOfCore(coreCounter);
      int frequency = oldFrequencies.at(coreCounter);
      float utilization =
          performanceCounters->getUtilizationOfCore(coreCounter);

      cout << "[Scheduler][DVFS_Sched_FREQ_HS]: Core " << setw(2) << coreCounter
           << ":";
      cout << " P=" << fixed << setprecision(3) << power << " W";
      cout << " f=" << frequency << " MHz";
      cout << " T=" << fixed << setprecision(1) << temperature << " °C";
      cout << " utilization=" << fixed << setprecision(3) << utilization;

      double util = pelt->get_utilisation(coreCounter);
      double new_freq = pelt->get_frequency(util, maxFrequency);
      
      // adjust frequency based on current temperature
      if (tlt){
        cout << endl;
        frequencies.at(coreCounter) = minFrequency;
      }else{
        cout << " new freq=" << (int) new_freq << endl;
        frequencies.at(coreCounter) = (int) new_freq;
      }
    } else {
      frequencies.at(coreCounter) = minFrequency;
    }
  }
  return frequencies;
}

bool DVFSchedFreqHS::throttle() {
  if (performanceCounters->getPeakTemperature() > dtmCriticalTemperature) {
    if (!in_throttle_mode) {
      cout << "[Scheduler][sched_freq-DTM]: detected thermal violation" << endl;
    }
    in_throttle_mode = true;
  } else if (performanceCounters->getPeakTemperature() <
             dtmRecoveredTemperature) {
    if (in_throttle_mode) {
      cout << "[Scheduler][sched_freq-DTM]: thermal violation ended" << endl;
    }
    in_throttle_mode = false;
  }
  return in_throttle_mode;
}
