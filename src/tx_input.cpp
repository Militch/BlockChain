//
// Created by militch on 2019-05-14.
//

#include "tx_input.h"

bool TXInput::can_unlock_out_with(const string& unlocking_data){
    return this->script_sign == unlocking_data;
}
