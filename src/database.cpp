#include "database.h"
#include <stdexcept>



/*
    DataBase class implementation
    provides methods to execute SQL queries and commands on the database
*/
DataBase::DataBase(const std::string filename) {
        if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_close(db);
            throw std::runtime_error("Can't open database: " + err);
        }
}

DataBase::~DataBase() {
        if (db) sqlite3_close(db);
}


void DataBase::execute(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("Execution error: " + err);
    }
}


void DataBase::execute(const std::string& sql, const std::vector<std::string>& params) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        throw std::runtime_error("Preparation error: " + err);
    }

    for (size_t i = 0; i < params.size(); ++i) {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error("Binding error: " + err);
        }
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("Execution error: " + err);
    }

    sqlite3_finalize(stmt);
}


std::vector<std::vector<std::string>> DataBase::query(const std::string& sql) {
    sqlite3_stmt* stmt;
    std::vector<std::vector<std::string>> results;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        throw std::runtime_error("Preparation error: " + err);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        std::vector<std::string> row;
        for (int i = 0; i < cols; ++i) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(text ? text : "");
        }
        results.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    return results;
}


std::vector<std::vector<std::string>> DataBase::query(const std::string& sql, const std::vector<std::string>& params) {
    std::vector<std::vector<std::string>> res;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        throw std::runtime_error("Preparation error: " + err);
    }

    for (size_t i = 0; i < params.size(); ++i) {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error("Binding error: " + err);
        }
    }
    int columns = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < columns; i++) {
            const char* col = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(col ? col : "");
        }
        res.push_back(row);
    }

    sqlite3_finalize(stmt);
    return res;
}