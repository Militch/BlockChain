//
// Created by militch on 2019-05-16.
//

#include "db_helper.h"
#include <unistd.h>

#include "leveldb/write_batch.h"
using namespace std;

DBHelper* DBHelper::instance = nullptr;

DBHelper::DBHelper(leveldb::DB* db) {
    this->db = db;
}

DBHelper *DBHelper::get_instance() {
    if (instance == nullptr){
        string current_path = string(getcwd(nullptr,0));
        string db_path = current_path + "/db";
        leveldb::DB* db;
        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::Status status = leveldb::DB::Open(options, db_path, &db);
        assert(status.ok());
        instance = new DBHelper(db);
    }
    return instance;
}

int DBHelper::put(const std::string &key, const std::string &value) {
    leveldb::WriteBatch batch;
    batch.Put(key,value);
    batch.Put("_LAST_KEY",key);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
    if (!s.ok()) return -1;
    return 0;
}


std::vector<std::string> DBHelper::select(const std::string &key) {
    vector<string> vs;
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
    }
    return vector<string>();
}

std::string DBHelper::last_key() {
    string value;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), "_LAST_KEY", &value);
    if (!s.ok()) return "";
    return value;
}

DBHelper::Iterator *DBHelper::iterator() {
    auto iter = new Iterator();
    iter->iter = db->NewIterator(leveldb::ReadOptions());
    iter->iter->SeekToFirst();
    return iter;
}

int DBHelper::del(const std::string& key) {
    string last = last_key();
    leveldb::WriteBatch batch;
    if (last == key){
        batch.Delete("_LAST_KEY");
    }
    batch.Delete(key);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
    if (!s.ok()) return -1;
    return 0;
}
int DBHelper::clear() {
    return 0;
}


bool DBHelper::Iterator::has_next() {
    if (iter->Valid()){
        string k = iter->key().ToString();
        if (k == "_LAST_KEY"){
            iter->Next();
            return iter->Valid();
        }
    }
    return iter->Valid();
}

void DBHelper::Iterator::next() {
    iter->Next();
}

std::string DBHelper::Iterator::key() {
    return iter->key().ToString();
}

std::string DBHelper::Iterator::value() {
    return iter->value().ToString();
}

