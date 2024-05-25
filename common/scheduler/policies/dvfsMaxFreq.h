/**
 * This header implements the max. freq DVFS policy
 */

#ifndef __DVFS_MAXFREQ_H
#define __DVFS_MAXFREQ_H

#include <vector>
#include "dvfspolicy.h"
#include "pelt.h"

class DVFSMaxFreq : public DVFSPolicy {
public:
    DVFSMaxFreq(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);

private:
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    int maxFrequency;
};

class DVFSMinMaxFreq : public DVFSPolicy {
public:
    DVFSMinMaxFreq(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency, int minFrequency);
    virtual std::vector<int> getFrequencies(const std::vector<int> &oldFrequencies, const std::vector<bool> &activeCores);
    

private:
    const PerformanceCounters *performanceCounters;
    unsigned int coreRows;
    unsigned int coreColumns;
    int maxFrequency;
    int minFrequency;
};

class DVFSchedFreq : public DVFSPolicy {
public:
    DVFSchedFreq(const PerformanceCounters *performanceCounters, int coreRows, int coreColumns, int maxFrequency, int minFrequency, float dtmCriticalTemperature, float dtmRecoveredTemperature,PELT *pelt);
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
    bool throttle();
    void addFrequency(int coreCounter);
};


#endif
