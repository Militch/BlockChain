//
// Created by militch on 2019-05-16.
//

#ifndef BLOCKCHAIN_DB_HELPER_H
#define BLOCKCHAIN_DB_HELPER_H

#include <iostream>
#include <leveldb/db.h>
#include <string>
#include <vector>

class DBHelper {

public:
    class Iterator {
    public:
        leveldb::Iterator* iter;
        bool has_next();
        void next();
        std::string key();
        std::string value();
    };
    static DBHelper* get_instance();
    int put(const std::string& key, const std::string& value);
    std::string last_key();
    Iterator* iterator();
    int del(const std::string& key);
    int clear();
    std::vector<std::string> select(const std::string& key);
private:
    leveldb::DB* db;
    static DBHelper* instance;
    explicit DBHelper(leveldb::DB* db);
};


#endif //BLOCKCHAIN_DB_HELPER_H
