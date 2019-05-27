//
// Created by militch on 2019-05-17.
//

#ifndef BLOCKCHAIN_CRYPT_H
#define BLOCKCHAIN_CRYPT_H

#include <iostream>
#include <string>
#include <secp256k1.h>

namespace Crypt {
    const static secp256k1_context* _cxt = secp256k1_context_create(SECP256K1_CONTEXT_SIGN|SECP256K1_CONTEXT_VERIFY);
    const static size_t LEN_SEC_KEY = 32;
    const static size_t LEN_PUB_KEY = 65;
    const static size_t LEN_SIGN = 72;
    void test();
    void test2();

    /**
     * 创建私钥与公钥
     * @param sec_key 输出私钥
     * @param pub_key 输出公钥
     * @return 1为成功，否则失败
     */
    int create(unsigned char* sec_key, unsigned char* pub_key);

    int load(char* sec_key_hex, size_t sec_key_hex_len,char* pub_key_hex, size_t pub_key_hex_len,
            unsigned char* out_sec_key, unsigned char* out_pub_key);
    /**
     * 使用私钥对消息签名
     * @param sec_key 私钥
     * @param msg 消息
     * @param out 输出
     * @param out_len 输出数组大小
     * @return 1为成功，否则失败
     */
    int sign(unsigned char* sec_key, unsigned char* msg, unsigned char* out, size_t* out_len);
    /**
     * 使用公钥校验信息
     * @param pub_key 公钥
     * @param pub_key_len 公钥数组长度
     * @param sign 签名
     * @param sign_len 签名字节长度
     * @param msg32 内容
     * @return 1为成功，否则失败
     */
    int verify(unsigned char* pub_key, size_t pub_key_len,unsigned char* sign, size_t sign_len,
            const unsigned char *msg32);

    /**
     * 字节数组转十六进制 char 数组
     * @param input 输入字节数组
     * @param len 字节数组大小
     * @param output 输出字节数组，请保证数组大小大于 len * 2 + 1
     * @return 1为成功，否则失败
     */
    int bytes2hex(const unsigned char* input,size_t len, char* output);
    std::string bytes2hex_str(const unsigned char* input,size_t len);
    int hex2bytes(const char* hex,size_t hex_len, unsigned char* out);
    int hex_string2bytes(const std::string& hex,unsigned char* out);
}
#endif //BLOCKCHAIN_CRYPT_H
