#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <sqlite3.h>

// database file name
const std::string DB_FILE = "db.db";

class DataBase {
    sqlite3* db;
public:
    DataBase(const std::string filename) {
        if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_close(db);
            throw std::runtime_error("Can't open database: " + err);
        }
    }

    ~DataBase() {
        if (db) sqlite3_close(db);
    }

    void execute(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string err = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("Execution error: " + err);
        }
    }

    void execute(const std::string& sql, const std::vector<std::string>& params) {
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

    std::vector<std::vector<std::string>> query(const std::string& sql) {
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


    std::vector<std::vector<std::string>> query(const std::string& sql, const std::vector<std::string>& params) {
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


    DataBase(const DataBase&) = delete;
    DataBase& operator=(const DataBase&) = delete;
};


/*
    API password manager class
    provides methods to add, show, find and delete passwords in the database
*/
class PasswordManager {
private:
    DataBase& db;
public:
    PasswordManager(DataBase& database) :
    db(database) {}


    void add_password(const std::string& service,
                      const std::string& login,
                      const std::string& password) {
        std::string query = R"(
                            INSERT INTO passwords 
                            (service, login, password) VALUES (?, ?, ?)
                            )";
        db.execute(query, {service, login, password});

    }
    

    void show_passwords() {
        auto res = db.query("SELECT service, login, password FROM passwords");
        std::for_each(res.begin(), res.end(), [](const std::vector<std::string>& row) {
            std::cout << "Service: " << row[0] << " | Login: " << row[1] << " | Password: " << row[2] << '\n';
        });
    }


    std::vector<std::vector<std::string>> find_password(const std::string& service) {
        std::vector<std::vector<std::string>> res;
        res = db.query("SELECT service, login, password FROM passwords WHERE service = ?", {service});
        return res;
    }


    void delete_password(const std::string& line_to_delete) {
        db.execute("DELETE FROM passwords WHERE service = ?", {line_to_delete});
    }

};

// menu
void show_menu() {
    std::cout << "========== Terminal Password Manager ==========\n"
            << "1. Add password\n"
            << "2. Show all services\n"
            << "3. Find password\n"
            << "4. Delete password\n"
            << "5. Exit\n"
            << "> ";
}

int main() {
    DataBase db(DB_FILE);
    PasswordManager manager(db);
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS passwords (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        service TEXT,
        login TEXT,
        password TEXT)
        )");

    while (true) {
        show_menu();
        int choice;
        std::cin >> choice;
        switch (choice)
        {
        case 1: {
            std::string service, login, password;
            std::cout << "input data in format (service login password)\n";
            std::cin >> service >> login >> password;
            manager.add_password(service, login, password);
            break;
        }

        case 2:
            std::cout << "=============== All passwords =================\n";
            manager.show_passwords();
            std::cout << "===============================================\n";
            break;
        case 3: {
            std::string service_name;
            std::cout << "Enter service name: ";
            std::cin >> service_name;
            std::vector<std::vector<std::string>> found_passwords = manager.find_password(service_name);
            if (!found_passwords.empty()) {
                std::cout << "============== Founded passwords ==============\n";
            std::for_each(found_passwords.begin(), found_passwords.end(), [](const std::vector<std::string>& row) {
                std::cout << "Service: " << row[0] << " | Login: " << row[1] << " | Password: " << row[2] << '\n';
            });
                std::cout << "===============================================\n";
            }
            else {
                std::cout << "Password not found.\n";
            }
            break;
        }
        case 4: {
            std::cout << "Input string password to delete:" << std::endl;
            std::string password_to_delete;
            std::cin.ignore();
            std::getline(std::cin, password_to_delete);
            manager.delete_password(password_to_delete);
            break;
        }

        case 5:
        {
            return 0;
            break;
        }
        default: {
            std::cout << "Please, input exist number!\n";
            break;
        }
        }
    }
}