//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_PROOF_OF_WORK_H
#define BLOCKCHAIN_PROOF_OF_WORK_H

#include <iostream>
#include "util/sha256.h"
#include "block.h"

using namespace std::chrono;

inline long current_timestamps(){
    chrono::milliseconds m = duration_cast<chrono::milliseconds>(system_clock::now().time_since_epoch());
    return m.count();
}

inline string calculate_hash(long index, long timestamp, string prevHash){
    string record = "";
    record += to_string(index)
              + to_string(timestamp)
              + prevHash;
    return sha256(record);
}

class ProofOfWork{
public:
    const static long TARGET_BITS = 3;
    ProofOfWork(Block* block){
        this->block_ = block;
        this->target_ = 1UL << (31UL - TARGET_BITS);
    }
    string run();
private:
    Block* block_;
    unsigned long target_;
    string calculate_hash();
    string tx_hash();
    string prepareData();
};


#endif //BLOCKCHAIN_PROOF_OF_WORK_H
