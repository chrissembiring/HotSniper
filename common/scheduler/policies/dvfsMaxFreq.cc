#include "dvfsMaxFreq.h"
#include <iomanip>
#include <iostream>

using namespace std;

DVFSMaxFreq::DVFSMaxFreq(const PerformanceCounters *performanceCounters,
                         int coreRows, int coreColumns, int maxFrequency)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency) {}

std::vector<int>
DVFSMaxFreq::getFrequencies(const std::vector<int> &oldFrequencies,
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

      cout << "[Scheduler][DVFS_MAX_FREQ]: Core " << setw(2) << coreCounter
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

DVFSMinMaxFreq::DVFSMinMaxFreq(const PerformanceCounters *performanceCounters,
                               int coreRows, int coreColumns, int maxFrequency,
                               int minFrequency)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency),
      minFrequency(minFrequency) {}

std::vector<int>
DVFSMinMaxFreq::getFrequencies(const std::vector<int> &oldFrequencies,
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

      cout << "[Scheduler][DVFS_MINMAX_FREQ]: Core " << setw(2) << coreCounter
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

DVFSchedFreq::DVFSchedFreq(const PerformanceCounters *performanceCounters,
                           int coreRows, int coreColumns, int maxFrequency,
                           int minFrequency, float dtmCriticalTemperature,
                           float dtmRecoveredTemperature, PELT *pelt)
    : performanceCounters(performanceCounters), coreRows(coreRows),
      coreColumns(coreColumns), maxFrequency(maxFrequency),
      minFrequency(minFrequency),
      dtmCriticalTemperature(dtmCriticalTemperature),
      dtmRecoveredTemperature(dtmRecoveredTemperature), pelt(pelt) {}

std::vector<int>
DVFSchedFreq::getFrequencies(const std::vector<int> &oldFrequencies,
                             const std::vector<bool> &activeCores) {
  std::vector<int> frequencies(coreRows * coreColumns);

  // perform add Frequency operation
  for (unsigned int coreC = 0; coreC < coreRows * coreColumns; coreC++)
          addFrequency(coreC);
  
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

      cout << "[Scheduler][DVFS_Sched_FREQ]: Core " << setw(2) << coreCounter
           << ":";
      cout << " P=" << fixed << setprecision(3) << power << " W";
      cout << " f=" << frequency << " MHz";
      cout << " T=" << fixed << setprecision(1) << temperature << " °C";
      cout << " utilization=" << fixed << setprecision(3) << utilization;

      double util = pelt->get_utilisation(coreCounter);
      double new_freq = pelt->get_frequency(util, maxFrequency) + pelt->addFreq[coreCounter];

      // adjust frequency based on current temperature
      if (tlt) {
        cout << endl;
        frequencies.at(coreCounter) = minFrequency;
        pelt->addFreq[coreCounter] = 0;
      } else {
        cout << " new freq=" << (int)new_freq << endl;
        frequencies.at(coreCounter) = (int)new_freq;
      }
    } else {
      frequencies.at(coreCounter) = minFrequency;
    }
  }
  return frequencies;
}

void DVFSchedFreq::addFrequency(int coreCounter){
        float thresholdTemperature = this->dtmCriticalTemperature - 5;
        int previousCore = pelt->prev_core_mapping[coreCounter];

        if (performanceCounters->getTemperatureOfCore(previousCore) > thresholdTemperature)
                pelt->addFreq[coreCounter] -= 40;
        else
                pelt->addFreq[coreCounter] += 20;
}

bool DVFSchedFreq::throttle() {
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
