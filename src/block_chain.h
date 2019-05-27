//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_BLOCK_CHAIN_H
#define BLOCKCHAIN_BLOCK_CHAIN_H


#include <stdio.h>
#include <vector>
#include "block.h"
#include "proof_of_work.h"
#include "util/db_helper.h"
class BlockChain{
public:
    int create(const string& address);
    void print_last_block();
    vector<Block*> get_chain();
    Block* get_last();
    // 根据钱包地址查找所有未消费的交易
    vector<Transaction*> find_unspent_txs(string address);
    // 根据钱包地址查找对应的所有未消费的交易输出
    vector<TXOutput*> find_utxo(string address);

    // 转账
    int send(const string& from,const string& to, float amount);
    string last_block;
    static BlockChain* create(char*** argv);
    vector<Block*> get_all_blocks();
    void del();

private:
    const static string ZERO_HASH;
    vector<Block*> list;
//    DBHelper* db;
    // 查找可以满足指定金额消费的交易记录
    void find_spendable_txs(string address, int amount);
    static Block* create_genesis_block(const string& address);
    static Block* create_block(const string& prev_hash, vector<Transaction*>& txs);

    void add_block(Block &block);
    static Block* json2block(const string& key,const string& json);
};
#endif //BLOCKCHAIN_BLOCK_CHAIN_H
