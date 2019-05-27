#include <iostream>
#include <gflags/gflags.h>
#include <map>
#include "block_chain.h"
#include <openssl/md5.h>
//#include <zconf.h>
#include <unistd.h>
#include "util/crypt.h"
#include "wallet_helper.h"

DEFINE_bool(mine, true, "开启挖矿");


void _md5(const std::string &srcStr, std::string &encodedStr, std::string &encodedHexStr){
    unsigned char mdStr[33] = {0};
    MD5((const unsigned char *)srcStr.c_str(), srcStr.length(), mdStr);
    encodedStr = std::string((const char *)mdStr);
    char buf[65] = {0};
    char tmp[3] = {0};
    for (int i = 0; i < 32; i++){
        sprintf(tmp, "%02x", mdStr[i]);
        strcat(buf, tmp);
    }
    buf[32] = '\0';
    encodedHexStr = std::string(buf);
}

void handle_mine(BlockChain* bc){
    Block* last_block = bc->get_last();
    cout << "正在挖矿中..." << endl;
    long start_time = current_timestamps();
//    bc->add_block(last_block->hash, last_block->transactions);
    float elapsed_time = current_timestamps() - start_time;
    cout << "获得奖励 +10 ，耗时：(" << to_string((float)(elapsed_time/1000)) << "s)" << endl;
    bc->print_last_block();
}

void handle_balance(){

}

int get_balance(BlockChain* bc,string address){
    auto tx_outs = bc->find_utxo(address);
    int balance = 0;
    for (auto tx_out : tx_outs) {
        balance += tx_out->value;
    }
    return balance;
}
void get_cwd(){
    char* result = getcwd(nullptr,0);
    cout << "cwd: " << result;
}

int main(int argc, char* argv[]) {
    auto bc = new BlockChain();
    Wallet* wallet = nullptr;
    string command;
    do {
        cout << "> ";
        cin >> command;
        if (command == "mine"){
            handle_mine(bc);
        }else if(command == "cc"){
            if (wallet == nullptr){
                cout << "请先创建钱包地址" << endl;
                continue;
            }
            bc->create(wallet->address);
        }else if(command == "create"){
            wallet = WalletHelper::create();
            cout << "钱包创建成功! address: " << wallet->address << endl;
        }else if(command == "balance"){
            if (wallet == nullptr){
                cout << "请先创建钱包地址" << endl;
                continue;
            }
            int balance = get_balance(bc,wallet->address);
            cout << "当前余额: " << balance << endl;
        }else if(command == "send"){
            string address;
            cout << "请输入对方钱包地址: ";
            cin >> address;
            float amount;
            cout << "请输入转账金额: ";
            cin >> amount;
            int result = bc->send(wallet->address, address, amount);
            if (result == 0){
                cout << "交易成功" << endl;
            }else if(result == -1){
                cout << "交易失败, 余额不足" << endl;
            }
        }else if(command == "list"){
            bc->get_all_blocks();
        }else if(command == "del"){
            bc->del();
        }else if(command == "clear") {
//            DBHelper::get_instance()->last_key()
        }else if(command == "md5"){
            Crypt::test();
            char a[] = "0123456789abcdefg";
            cout << "md5: " << strchr(a,'a') << endl;
        }else if(command == "cwd"){
            get_cwd();
        }else if(command != "exit"){
            cout << "请输入正确的命令，或输入 exit 退出" << endl;
        }
    } while (command != "exit");
    return 0;
}

//int main(int argc, char* argv[]){
//    Crypt::test();
//    Crypt::test2();
////    const char* hex = "6c6f7665";
////    unsigned char out[5] = {0};
////    Crypt::hex_string2bytes(hex,out);
//}
