#include "database.h"
#include "password_manager.h"
#include <iostream>
#include <algorithm>

// database file name
const std::string DB_FILE = "db.db";


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