//
// Created by militch on 2019-05-14.
//

#include "tx_output.h"

bool TXOutput::can_be_unlocked_with(string unlocking_data){
    return this->script_pub_key == unlocking_data;
}
