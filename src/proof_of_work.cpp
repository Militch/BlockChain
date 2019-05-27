//
// Created by militch on 2019-05-14.
//

#include "proof_of_work.h"
#include <vector>

string ProofOfWork::calculate_hash(){
    string record;
    record += to_string(0)
              + to_string(this->block_->timestamp)
              + this->block_->prev_hash
              + this->tx_hash()
              + to_string(this->block_->nonce);
    return sha256(record);
}
string ProofOfWork::prepareData(){
    string t;
    for (int i=0; i<5; i++) {
        t += "0";
    }
    return t;
}
string ProofOfWork::run(){
    unsigned long* nonce = &this->block_->nonce;
    string shaHex;
    while (*nonce < LONG_MAX) {
        string hash = sha256(block_->serialized());
        string target = this->prepareData();
        string c = hash.substr(0,target.length());
        if (target == c) {
            return hash;
        }
        *nonce += 1;
    }
    return shaHex;
}

string ProofOfWork::tx_hash(){
    Block* block = this->block_;
    int tx_size = (int) block->transactions.size();
    string buff;
    for (int i=0; i<tx_size; i++) {
        buff += block->transactions[i]->id;
    }
    return buff;
}
