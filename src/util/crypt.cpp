//
// Created by militch on 2019-05-17.
//

#include "crypt.h"
#include <openssl/rand.h>
#include "sha256.h"

unsigned int hex_char2num(char c){
    const char* hex_digits = "0123456789abcdef";
    const char* match = std::find(hex_digits,hex_digits+16,c);
    return ((hex_digits+16) == match)? 0 : (match-hex_digits);
}
void Crypt::test2() {
    unsigned char sec_key[LEN_SEC_KEY];
    unsigned char pub_key[LEN_PUB_KEY];
    const char* sec_key_hex = "107b5ca1f05dc4a233fa56ce78d668ce602978bb6c986d24c537b0dede8979f7";
    const char* pub_key_hex = "04fda6f4d825a347bc03164361f7952a5f70a0a7e2059175d028784e45b65bf19661a943d472ec24ef12beec71be28da5ee6ffca95ee1a968f4400d010e25232c4";
    std::cout << "私钥: "<< sec_key_hex << std::endl << "公钥: " << pub_key_hex << std::endl;
    // 准备签名数据
    std::string msg_str = sha256("aaa");
    auto *msg =new unsigned char[msg_str.length()+1];
    strcpy((char *)msg,msg_str.c_str());
    size_t sign_out_len = LEN_SIGN;
    unsigned char sign_out[sign_out_len];
    assert(sign(sec_key,msg,sign_out,&sign_out_len));
    std::string hex_sign = bytes2hex_str(sign_out,sign_out_len);
    std::cout << "签名: "<< hex_sign << std::endl;
    assert(verify(pub_key,LEN_PUB_KEY,sign_out,sign_out_len,msg));
    std::cout << "校验成功"<< std::endl;
}
void Crypt::test() {
    unsigned char sec_key[LEN_SEC_KEY];
    unsigned char pub_key[LEN_PUB_KEY];
    // 创建密钥对（私钥&公钥）
    assert(create(sec_key,pub_key));
    // 将私钥转换成16进制输出
    std::string hex_sec_key = bytes2hex_str(sec_key,LEN_SEC_KEY);
    // 将公钥转换成16进制输出
    std::string hex_pub_key = bytes2hex_str(pub_key,LEN_PUB_KEY);
    std::cout << "私钥: "<< hex_sec_key << std::endl << "公钥: " << hex_pub_key << std::endl;

    std::string msg_str = sha256("aaa");
    auto *msg =new unsigned char[msg_str.length()+1];
    strcpy((char *)msg,msg_str.c_str());
    size_t sign_out_len = LEN_SIGN;
    unsigned char sign_out[sign_out_len];
    assert(sign(sec_key,msg,sign_out,&sign_out_len));
    std::string hex_sign = bytes2hex_str(sign_out,sign_out_len);
    std::cout << "签名: "<< hex_sign << std::endl;
    assert(verify(pub_key,LEN_PUB_KEY,sign_out,sign_out_len,msg));
    std::cout << "校验成功"<< std::endl;
}
int Crypt::load(char *sec_key_hex, size_t sec_key_hex_len, char *pub_key_hex, size_t pub_key_hex_len,
                unsigned char *out_sec_key, unsigned char *out_pub_key) {
    hex2bytes(sec_key_hex,sec_key_hex_len,out_sec_key);
    hex2bytes(pub_key_hex,pub_key_hex_len,out_pub_key);
    return 1;
}
int Crypt::create(unsigned char* sec_key, unsigned char* pub_key) {
    std::string hex;
    RAND_bytes(sec_key,32);
    if (!secp256k1_ec_seckey_verify(_cxt,sec_key)) return 0;
    secp256k1_pubkey pub_key_n;
    if(!secp256k1_ec_pubkey_create(_cxt,&pub_key_n,sec_key)) return 0;
    size_t pub_key_len = 65;
    if(!secp256k1_ec_pubkey_serialize(_cxt,pub_key,&pub_key_len,&pub_key_n,SECP256K1_EC_UNCOMPRESSED)) return 0;
    return 1;
}

int Crypt::bytes2hex(const unsigned char* input, size_t len, char* output){
    const char *hex_digits = "0123456789abcdef";
    size_t i,j;
    for (i=j=0; i < len; i++) {
        const unsigned char byte = input[i];
        unsigned int index = byte >> 4u;
        char hex_char = hex_digits[index];
        *(output + j++) = hex_char;
        index = (byte & 0x0fu);
        hex_char = hex_digits[index];
        *(output + j++) = hex_char;
    }
    *(output + j) = '\0';
    return 1;
}
int Crypt::hex2bytes(const char *hex, size_t hex_len, unsigned char* out) {
    size_t i,j;
    for (i = j = 0; i < hex_len; i+=2) {
        unsigned int p1_num = hex_char2num(*(hex+i));
        unsigned int p2_num = hex_char2num(*(hex+i+1));
        p1_num = p1_num << 4u;
        *(out+j++) = p1_num | p2_num;
    }
    *(out+j) = '\0';
    return 1;
}
int Crypt::hex_string2bytes(const std::string& hex, unsigned char *out) {
    return hex2bytes(hex.c_str(),hex.size(),out);
}
std::string Crypt::bytes2hex_str(const unsigned char *input, size_t len) {
    char temp[len*2+1];
    assert(bytes2hex(input,len,temp));
    return std::string(temp);
}

int Crypt::sign(unsigned char *sec_key, unsigned char* msg, unsigned char* out, size_t* out_len) {
    secp256k1_ecdsa_signature s;
    if(!secp256k1_ecdsa_sign(_cxt,&s,msg,sec_key,nullptr, nullptr)) return 0;
    if(!secp256k1_ecdsa_signature_serialize_der(_cxt,out,out_len,&s)) return 0;
    return 1;
}
int Crypt::verify(unsigned char *pub_key, size_t pub_key_len, unsigned char *sign, size_t sign_len,
                  const unsigned char *msg32) {
    secp256k1_pubkey pub_key_n;
    if(!secp256k1_ec_pubkey_parse(_cxt,&pub_key_n,pub_key,pub_key_len)) return 0;
    secp256k1_ecdsa_signature s;
    if(!secp256k1_ecdsa_signature_parse_der(_cxt,&s,sign,sign_len)) return 0;
    if(!secp256k1_ecdsa_verify(_cxt,&s,msg32,&pub_key_n)) return 0;
    return 1;
}