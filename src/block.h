//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_BLOCK_H
#define BLOCKCHAIN_BLOCK_H


#include <string>
#include <stdio.h>
#include <vector>
#include "transaction.h"

using namespace std;

class Block{
public:
    unsigned long timestamp;
    unsigned long nonce;
    string prev_hash;
    string hash;
    vector<Transaction*> transactions;
    string serialized();
};


#endif //BLOCKCHAIN_BLOCK_H
