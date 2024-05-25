/**
 * This header implements the max. freq DVFS policy
 */

#ifndef __DVFS_MAXFREQ_HS_H
#define __DVFS_MAXFREQ_HS_H

#include <vector>
#include "dvfspolicy.h"
#include "pelt.h"
#include "../../../hotspot/temperature.h"

class DVFSMaxFreqHS : public DVFSPolicy {
public:
    DVFSMaxFreqHS(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);

private:
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    int maxFrequency;
};

class DVFSMinMaxFreqHS : public DVFSPolicy {
public:
    DVFSMinMaxFreqHS(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency, int minFrequency);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);
    

private:
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    int maxFrequency;
    int minFrequency;
};

class DVFSchedFreqHS : public DVFSPolicy {
public:
    DVFSchedFreqHS(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency, int minFrequency, float dtmCriticalTemperature, float dtmRecoveredTemperature,PELT *pelt);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);

private:
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    int maxFrequency;
    int minFrequency;
    float dtmCriticalTemperature;
    float dtmRecoveredTemperature;
    PELT *pelt;
    bool in_throttle_mode = false;
    RC_model_t model;
    bool throttle();
    void addFrequency(int coreCounter);
};


#endif