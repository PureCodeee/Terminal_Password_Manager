#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>


/*
    DataBase class declaration
    provides methods to execute SQL queries and commands on the database
*/
class DataBase {
    sqlite3* db;
public:
    DataBase(const std::string filename);
    ~DataBase();

    void execute(const std::string& sql);
    void execute(const std::string& sql, const std::vector<std::string>& params);

    std::vector<std::vector<std::string>> query(const std::string& sql);
    std::vector<std::vector<std::string>> query(const std::string& sql, const std::vector<std::string>& params);

    DataBase(const DataBase&) = delete;
    DataBase& operator=(const DataBase&) = delete;
};