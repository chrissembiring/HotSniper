#include "pelt.h"

PELT::PELT(int coreCount, int dvfsEpoch){
        this->coreCount = coreCount;
        this->n = dvfsEpoch/1100;
        for (int i = 0; i < coreCount; i++){ 
                core_utils.push_back(new std::deque<double>()); 
                this->prev_core_mapping.push_back(i);
                this->addFreq.push_back(0); // set additional frequency as 0
        }
};

PELT::~PELT(){
        for (int i = 0; i < coreCount; i++)
                delete core_utils[i];
};

void PELT::append(int coreCounter, double interval_util){
        std::deque<double> * core_util_dq = core_utils[coreCounter];
        if (core_util_dq->size() == n){
                core_util_dq->pop_back();
                core_util_dq->push_front(interval_util);
        } else {
                core_util_dq->push_front(interval_util);
        }
};

double PELT::get_utilisation(int coreCounter){
        double util = 0;
        double divisor = 1;

        // make use of history of previous mapped core
        int prevMappedCore = this->prev_core_mapping[coreCounter];

        double tmp_y = y;
        std::deque<double> * core_util_dq = core_utils[prevMappedCore];
        
        util = (*core_util_dq)[0];
        for (unsigned long i = 1; i < core_util_dq->size(); i++){
                util += tmp_y * (*core_util_dq)[i];
                divisor += tmp_y;
                tmp_y = tmp_y * y;
        }

        divisor = divisor * 1100; // taking 1100 instead of 1024 because periodic is called every 100ns
        return util / divisor;
};

double PELT::get_frequency(double util, double maxFrequency){
        return 1.25 * maxFrequency * util;
}