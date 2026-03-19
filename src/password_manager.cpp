#include "password_manager.h"
#include <iostream>
#include <algorithm>


/*
    API password manager class declaration
    provides methods to add, show, find and delete passwords in the database
*/
PasswordManager::PasswordManager(DataBase& database)
    : db(database) {}


void PasswordManager::add_password(const std::string& service,
                    const std::string& login,
                    const std::string& password) {
    std::string query = R"(
                        INSERT INTO passwords 
                        (service, login, password) VALUES (?, ?, ?)
                        )";
    db.execute(query, {service, login, password});

}
    

void PasswordManager::show_passwords() {
    auto res = db.query("SELECT service, login, password FROM passwords");
    std::for_each(res.begin(), res.end(), [](const std::vector<std::string>& row) {
        std::cout << "Service: " << row[0] << " | Login: " << row[1] << " | Password: " << row[2] << '\n';
    });
}


std::vector<std::vector<std::string>> PasswordManager::find_password(const std::string& service) {
    std::vector<std::vector<std::string>> res;
    res = db.query("SELECT service, login, password FROM passwords WHERE service = ?", {service});
    return res;
}


void PasswordManager::delete_password(const std::string& line_to_delete) {
    db.execute("DELETE FROM passwords WHERE service = ?", {line_to_delete});
}


