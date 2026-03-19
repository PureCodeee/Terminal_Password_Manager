#pragma once
#include <string>
#include <vector>
#include "database.h"


/*
    API password manager class declaration
    provides methods to add, show, find and delete passwords in the database
*/

class PasswordManager {
private:
    DataBase& db;

public:
    PasswordManager(DataBase& database);

    void add_password(const std::string& service,
                      const std::string& login,
                      const std::string& password);

    void show_passwords();

    std::vector<std::vector<std::string>> find_password(const std::string& service);

    void delete_password(const std::string& password);
};