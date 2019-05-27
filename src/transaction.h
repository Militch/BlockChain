//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_TRANSACTION_H
#define BLOCKCHAIN_TRANSACTION_H


#include <stdio.h>
#include <vector>
#include "tx_input.h"
#include "tx_output.h"

class Transaction{
public:
    string id;
    vector<TXInput*> inputs;
    vector<TXOutput*> outputs;
    bool is_coin_base = false;
    string serialized();
    void make_id();
    static Transaction* new_coin_base_tx(const string& to, const string& data);
//    static Transaction* send(string from, string to, int amount, BlockChain* block_chain);

};

#endif //BLOCKCHAIN_TRANSACTION_H
