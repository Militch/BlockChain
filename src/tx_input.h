//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_TX_INPUT_H
#define BLOCKCHAIN_TX_INPUT_H

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class TXInput{
public:
    string tx_id;
    int tx_output_index;
    string script_sign;
    // 签名
    string signature;
    // 公钥
    string pub_key;

    // 是否能够解密数据
    bool can_unlock_out_with(const string& unlocking_data);
};

#endif //BLOCKCHAIN_TX_INPUT_H
