//
// Created by militch on 2019-05-14.
//

#include "transaction.h"
#include "util/sha256.h"
#include <jansson.h>

string Transaction::serialized() {
    json_t* json_tx = json_object();
    json_t* json_txs_outputs = json_array();
    json_t* json_txs_inputs = json_array();
    for(TXInput* in : this->inputs){
        json_t* json_txs_input = json_object();
        json_object_set_new(json_txs_input, "tx_id",json_string(in->tx_id.c_str()));
        json_object_set_new(json_txs_input, "tx_output_index",json_integer(in->tx_output_index));
        json_object_set_new(json_txs_input, "script_sign",json_string(in->script_sign.c_str()));
        json_array_append_new(json_txs_inputs, json_txs_input);
    }
    for(TXOutput * out: this->outputs){
        json_t* json_txs_output = json_object();
        const char* cc = to_string(out->value).c_str();
        json_object_set_new(json_txs_output, "value",json_string(cc));
        json_object_set_new(json_txs_output, "script_pub_key",json_string(out->script_pub_key.c_str()));
        json_array_append_new(json_txs_outputs,json_txs_output);
    }
    json_object_set_new(json_tx,"inputs",json_txs_inputs);
    json_object_set_new(json_tx,"outputs",json_txs_outputs);
    json_object_set_new(json_tx,"is_coin_base", json_boolean(this->is_coin_base));
    char *result = json_dumps(json_tx,0);
    return string(result);
}

Transaction *Transaction::new_coin_base_tx(const string& to, const string& data) {
    // 创建交易输入
    auto* input = new TXInput();
    input->tx_output_index = -1;
    input->script_sign = data;

    // 创建交易输出
    auto* output = new TXOutput();
    output->value = 10.00;
    output->script_pub_key = to;

    // 存储交易信息
    auto* tx = new Transaction();
    tx->inputs.push_back(input);
    tx->outputs.push_back(output);
    tx->is_coin_base = true;
    tx->make_id();
    return tx;
}

void Transaction::make_id() {
    string hash = sha256(this->serialized());
    this->id = hash;
}
