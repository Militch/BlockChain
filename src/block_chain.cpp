//
// Created by militch on 2019-05-14.
//

#include "block_chain.h"
#include <sstream>
#include <map>
#include <leveldb/db.h>
#include <unistd.h>
#include "util/db_helper.h"
#include <jansson.h>

using namespace std;

const string BlockChain::ZERO_HASH = "1000000000000000000000000000000000000000000000000000000000000000";
//BlockChain::BlockChain(){
//    vector<Transaction*> txs;
//    txs.push_back(Transaction::new_coin_base_tx("abc.tx", ""));
//    this->add_block("", txs);
//}

//void BlockChain::add_block(const string& prev_hash, vector<Transaction*>& txs){
//    long current_timestamp = current_timestamps();
//    // 构建新的区块
//    auto* block = new Block();
//    block->index = this->list.size();
//    block->timestamp = current_timestamp;
//    block->prev_hash = prev_hash;
//    block->transactions = txs;
//    block->nonce = 0;
//    // 创建工作难度证明
//    auto* pow = new ProofOfWork(block);
//    // 开始执行（挖矿）
//    block->hash = pow->run();
//    this->list.push_back(block);
//}
Block* BlockChain::get_last(){
    return this->list[this->list.size()-1];
}



vector<Block*> BlockChain::get_chain(){
    return this->list;
}
void BlockChain::print_last_block(){
    Block* last_block = this->list[this->list.size()-1];
    cout << "==================== 区块信息 ====================" << endl;
    cout << "index: " << 0<< endl
         << "timestamp: " << last_block->timestamp << endl
         << "nonce: " << last_block->nonce << endl
         << "prev_hash: " << last_block->prev_hash << endl
         << "hash: " << last_block->hash << endl;
    cout << "=================================================" << endl;
}

vector<Transaction*> BlockChain::find_unspent_txs(string address){
    vector<Transaction*> unspent_txs;
    map<string, vector<int>> m;
    DBHelper* dbHelper = DBHelper::get_instance();
    DBHelper::Iterator* iter = dbHelper->iterator();
    // 遍历链表中的所有区块，查找已被消费的交易
    for (;iter->has_next(); iter->next()) {
        Block* block = json2block(iter->key(), iter->value());
        // 遍历区块中的所有交易记录
        for(int j=0; j < block->transactions.size();j++){
            Transaction* tx = block->transactions[j];
            // 如果是 coin_base 交易，则跳过。
            if (tx->is_coin_base) {
                continue;
            }
            // 遍历交易记录中的所有输入
            for (int k=0; k < tx->inputs.size(); k++) {
                TXInput* input = tx->inputs[k];
                // 匹配能否被解锁
                if (input->can_unlock_out_with(address)) {
                    if (m.count(input->tx_id) > 0) {
                        auto it = m.find(input->tx_id);
                        vector<int> v = it->second;
                        v.push_back(input->tx_output_index);
                    }else{
                        vector<int> v;
                        v.push_back(input->tx_output_index);
                        m.insert(map<string, vector<int>>::value_type(input->tx_id,v));
                    }
                }
            }
        }
    }
    DBHelper::Iterator* iter2 = dbHelper->iterator();
    // 遍历链表中的所有区块，查找未消费的交易
    for (int i=0; iter2->has_next(); iter2->next()) {
        Block* block = json2block(iter2->key(),iter2->value());
        // 遍历区块中的所有交易记录
        for(int j=0; j < block->transactions.size();j++){
            Transaction* tx = block->transactions[j];
            auto it = m.find(tx->id);
            vector<int> v;
            if (it != m.end()){
                v = it->second;
            }
            // 遍历交易记录中的输出
            for (int k=0; k< tx->outputs.size(); k++) {
                if (!v.empty()) {
                    if (find(v.begin(), v.end(), k)!=v.end()) {
                        continue;
                    }
                }
                if (tx->outputs[k]->can_be_unlocked_with(address)) {
                    unspent_txs.push_back(tx);
                }
            }
        }
    }
    return unspent_txs;
}

vector<TXOutput*> BlockChain::find_utxo(string address){
    vector<TXOutput*> outs;
    vector<Transaction*> unspentTxs = this->find_unspent_txs(address);
    if (unspentTxs.empty()) {
        return outs;
    }
    for (int i=0; i<unspentTxs.size(); i++) {
        Transaction* tx = unspentTxs[i];
        for (int j=0; j<tx->outputs.size(); j++) {
            TXOutput* tx_output = tx->outputs[j];
            if (tx_output->can_be_unlocked_with(address)) {
                outs.push_back(tx_output);
            }
        }
    }
    return outs;
}

int BlockChain::send(const string& from,const string& to, float amount) {
    float accumulated = 0;
    // 查找未被消费的交易记录
    vector<Transaction*> unspent_txs = this->find_unspent_txs(from);
    map<string,vector<int>> unspent_outs;
    // 遍历交易记录累计额度
    for (auto tx : unspent_txs) {
        for (int i = 0; i < tx->outputs.size(); ++i) {
            auto output = tx->outputs[i];
            if (output->can_be_unlocked_with(from) && accumulated < amount){
                accumulated += output->value;
                vector<int> outs;
                auto iter = unspent_outs.find(tx->id);
                if (iter != unspent_outs.end()){
                    outs = iter->second;
                }
                outs.push_back(i);
                unspent_outs[tx->id] = outs;
                if (accumulated >= amount){
                    break;
                }
            }
        }
    }
    // 判断额度是否满足
    if (accumulated < amount){
        return -1;
    }
    vector<TXInput*> tx_inputs;
    for (auto &iter : unspent_outs){
        // 交易记录ID
        auto k = iter.first;
        // 交易输出索引
        auto v = iter.second;
        for (auto index : v) {
            auto tx_input = new TXInput();
            tx_input->tx_id = k;
            tx_input->tx_output_index = index;
            tx_input->script_sign = from;
            tx_inputs.push_back(tx_input);
        }
    }
    vector<TXOutput*> tx_outputs;
    auto tx_output = new TXOutput();
    tx_output->value = amount;
    tx_output->script_pub_key = to;
    tx_outputs.push_back(tx_output);
    if (accumulated > amount){
        // 找零记录
        auto tx_output_x = new TXOutput();
        tx_output_x->value = accumulated - amount;
        tx_output_x->script_pub_key = from;
        tx_outputs.push_back(tx_output_x);
    }
    auto transaction = new Transaction();
    transaction->inputs = tx_inputs;
    transaction->outputs = tx_outputs;
    transaction->make_id();
    DBHelper* dbHelper = DBHelper::get_instance();
    string lash_hash = dbHelper->last_key();
    vector<Transaction *> v(1, transaction);
    add_block(*create_block(lash_hash,v));
    return 0;
}
void BlockChain::find_spendable_txs(string address, int amount) {
    int accumulated = 0;
    // 查找未被消费的交易记录
    vector<Transaction*> unspent_txs = this->find_unspent_txs(address);
    map<string,vector<int>> unspent_outs;
    // 遍历交易记录累计额度
    for (auto tx : unspent_txs) {
        for (int i = 0; i < tx->outputs.size(); ++i) {
            auto output = tx->outputs[i];
            if (output->can_be_unlocked_with(address) && accumulated < amount){
                accumulated += output->value;
                vector<int> outs;
                auto iter = unspent_outs.find(tx->id);
                if (iter != unspent_outs.end()){
                    outs = iter->second;
                }
                outs.push_back(i);
                unspent_outs[tx->id] = outs;
                if (accumulated >= amount){
                    break;
                }
            }
        }
    }
}

BlockChain* BlockChain::create(char*** argv){
    DBHelper* helper = DBHelper::get_instance();
    string key = helper->last_key();
    if (key.empty()){
        auto bc = new BlockChain();
//        bc->create_genesis_block()
        helper->put("abc","def");
    }
    auto bc = new BlockChain();
    bc->last_block = "abc";
    return bc;
}

vector<Block *> BlockChain::get_all_blocks() {
    DBHelper* helper = DBHelper::get_instance();
    auto it = helper->iterator();
    for (; it->has_next(); it->next()) {
        cout << it->key() << ": "  << it->value() << endl;
    }
    return vector<Block *>();
}



int BlockChain::create(const string& address) {
    DBHelper* helper = DBHelper::get_instance();
    string key = helper->last_key();
    if (key.empty()) {
        add_block(*create_genesis_block(address));
        return 1;
    }
    return 0;
}

void BlockChain::add_block(Block &block) {
    DBHelper* helper = DBHelper::get_instance();
    helper->put(block.hash, block.serialized());
}

Block* BlockChain::create_genesis_block(const string& address){
    vector<Transaction*> txs;
    string genesis_coin_base_data = "16/May/2019";
    txs.push_back(Transaction::new_coin_base_tx(address, genesis_coin_base_data));
    return create_block(BlockChain::ZERO_HASH,txs);
}
Block* BlockChain::create_block(const string& prev_hash, vector<Transaction*>& txs){
    long current_timestamp = current_timestamps();
    // 构建新的区块
    auto* block = new Block();
    block->timestamp = current_timestamp;
    block->prev_hash = prev_hash;
    block->transactions = txs;
    block->nonce = 0;
    // 创建工作难度证明
    auto* pow = new ProofOfWork(block);
    // 开始执行（挖矿）
    block->hash = pow->run();
    return block;
}

void BlockChain::del() {
    DBHelper* helper = DBHelper::get_instance();
//    helper->del("0000063a8b1378eebb56cb3c0578b5e297764575e00eafd18f2db992f5d6a4a1");
    helper->del("00000ffbe1574a152d9d7497bc8c17b1821acf23bb7ac397a3e65c10bb5e554c");
//    helper->del("00000dbfc4cbf43546b7de12267f4ec932d28d8491ac6e4a6d7c6fcc638ffbdc");
}


Block* BlockChain::json2block(const string& key, const string& json) {
    json_error_t error;
    json_t *root = json_loads(json.c_str(), 0, &error);
    if (!root) return nullptr;
    auto block = new Block();
    block->hash = key;
    json_t *timestamp, *nonce, *prev_hash, *transactions;
    timestamp = json_object_get(root,"timestamp");
    nonce = json_object_get(root,"nonce");
    prev_hash = json_object_get(root,"prev_hash");
    transactions = json_object_get(root,"transactions");
    block->timestamp = json_integer_value(timestamp);
    block->nonce = json_integer_value(nonce);
    block->prev_hash = json_string_value(prev_hash);
    int sof_transactions = json_array_size(transactions);
    vector<Transaction*> txs;
    for(int i = 0; i < sof_transactions; i++){
        json_t* tx_j = json_array_get(transactions,i);

        json_t* tx_id, *tx_inputs, *tx_outputs, *is_coin_base;

        tx_id = json_object_get(tx_j,"id");
        tx_inputs = json_object_get(tx_j,"inputs");
        tx_outputs = json_object_get(tx_j,"outputs");
        is_coin_base = json_object_get(tx_j,"is_coin_base");

        auto tx = new Transaction();
        tx->id = json_string_value(tx_id);
        tx->is_coin_base = json_boolean_value(is_coin_base);

        vector<TXInput*> n_tx_inputs;
        vector<TXOutput*> n_tx_outputs;

        int sof_tx_inputs = json_array_size(tx_inputs);
        int sof_tx_outputs = json_array_size(tx_outputs);

        for (int j = 0; j < sof_tx_inputs; ++j) {
            json_t* tx_input_j = json_array_get(tx_inputs,j);

            json_t* tx_id_, *tx_output_index, *script_sign;
            tx_id_ = json_object_get(tx_input_j, "tx_id");
            tx_output_index = json_object_get(tx_input_j, "tx_output_index");
            script_sign = json_object_get(tx_input_j, "script_sign");

            auto n_tx_input = new TXInput();
            n_tx_input->tx_id = json_string_value(tx_id_);
            n_tx_input->tx_output_index = json_integer_value(tx_output_index);
            n_tx_input->script_sign = json_string_value(script_sign);
            n_tx_inputs.push_back(n_tx_input);
        }

        for (int j = 0; j < sof_tx_outputs; ++j) {
            json_t* tx_output_j = json_array_get(tx_outputs, j);

            json_t *tx_output_value, *tx_script_pub_key;
            tx_output_value = json_object_get(tx_output_j, "value");
            tx_script_pub_key = json_object_get(tx_output_j, "script_pub_key");
            string out_val = json_string_value(tx_output_value);
            auto n_tx_output = new TXOutput();
            n_tx_output->value = strtof(out_val.c_str(), nullptr);
            n_tx_output->script_pub_key = json_string_value(tx_script_pub_key);
            n_tx_outputs.push_back(n_tx_output);
        }
        tx->inputs = n_tx_inputs;
        tx->outputs = n_tx_outputs;
        txs.push_back(tx);
    }
    block->transactions = txs;
    return block;
}
