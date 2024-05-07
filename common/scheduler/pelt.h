#ifndef __PELT_H
#define __PELT_H

#include <vector>
#include <deque>

class PELT{
        public:
                PELT(int coreCount, int dvfsEpoch);
                double get_utilisation(int coreCounter);
                void append(int coreCounter, double interval_util);
                double get_frequency(double util, double maxFrequency);
                ~PELT();
        private:
                int coreCount;
                const double y = 0.9785;
                unsigned long n; // number of intervals within each dvfs epoch
                std::vector <std::deque<double> *> core_utils; 
};
#endif
