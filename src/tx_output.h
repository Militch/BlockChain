//
// Created by militch on 2019-05-14.
//

#ifndef BLOCKCHAIN_TX_OUTPUT_H
#define BLOCKCHAIN_TX_OUTPUT_H

#include <stdio.h>
#include <string>

using namespace std;

class TXOutput {
public:
    float value;
    string script_pub_key;
    // 使用公钥解密数据
    bool can_be_unlocked_with(string unlocking_data);
};

#endif //BLOCKCHAIN_TX_OUTPUT_H
