//
// Created by militch on 2019-05-14.
//

#include "block.h"
#include <jansson.h>

string Block::serialized() {
    json_t* root = json_object();
    json_object_set_new(root, "timestamp",json_integer(this->timestamp));
    json_object_set_new(root, "nonce", json_integer(this->nonce));
    json_object_set_new(root, "prev_hash", json_string(this->prev_hash.c_str()));
    json_t* json_txs = json_array();
    for (Transaction* tx : this->transactions){
        json_t* json_tx = json_object();
        json_t* json_txs_outputs = json_array();
        json_t* json_txs_inputs = json_array();
        for(TXInput* in : tx->inputs){
            json_t* json_txs_input = json_object();
            json_object_set_new(json_txs_input, "tx_id",json_string(in->tx_id.c_str()));
            json_object_set_new(json_txs_input, "tx_output_index",json_integer(in->tx_output_index));
            json_object_set_new(json_txs_input, "script_sign",json_string(in->script_sign.c_str()));
            json_array_append_new(json_txs_inputs, json_txs_input);
        }
        for(TXOutput * out: tx->outputs){
            json_t* json_txs_output = json_object();
            json_object_set_new(json_txs_output, "value",json_integer(out->value));
            json_object_set_new(json_txs_output, "script_pub_key",json_string(out->script_pub_key.c_str()));
            json_array_append_new(json_txs_outputs,json_txs_output);
        }
        json_object_set_new(json_tx, "id", json_string(tx->id.c_str()));
        json_object_set_new(json_tx,"is_coin_base",json_boolean(tx->is_coin_base));
        json_object_set_new(json_tx,"inputs",json_txs_inputs);
        json_object_set_new(json_tx,"outputs",json_txs_outputs);
        json_array_append_new(json_txs,json_tx);
    }
    json_object_set_new(root, "transactions", json_txs);
    char *result = json_dumps(root,0);
    return std::string(result);
}
