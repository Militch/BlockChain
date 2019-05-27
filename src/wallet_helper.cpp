//
// Created by militch on 2019-05-20.
//

#include "wallet_helper.h"
#include "util/crypt.h"

Wallet *WalletHelper::create() {
    unsigned char sec_key[Crypt::LEN_SEC_KEY];
    unsigned char pub_key[Crypt::LEN_PUB_KEY];
    assert(Crypt::create(sec_key,pub_key));
    char sec_key_hex[Crypt::LEN_SEC_KEY*2+1];
    char pub_key_hex[Crypt::LEN_PUB_KEY*2+1];
    Crypt::bytes2hex(sec_key,Crypt::LEN_SEC_KEY,sec_key_hex);
    Crypt::bytes2hex(pub_key,Crypt::LEN_PUB_KEY,pub_key_hex);
    auto* wallet = new Wallet();
    wallet->sec_key = sec_key_hex;
    wallet->pub_key = pub_key_hex;
    wallet->address = pub_key_hex;
    return wallet;
}
