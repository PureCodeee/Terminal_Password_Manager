#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>


class PasswordManager {
private:
    std::string path = "passwords.txt";
    std::string temp_filename = "temp.txt";

public:
    void add_password(std::string service, std::string login, std::string password) {
        std::ofstream passwords_file;

        if (!passwords_file.is_open()) {
            std::cerr << "Error opening file.\n";
            return;
        }

        passwords_file.open(path, std::ios::app);
        passwords_file << service << ' ' << login << ' ' << password << '\n';
        passwords_file.close();
    }
    

    void show_passwords() {
        std::ifstream passwords_file;
        std::string line;
        if (!passwords_file.is_open()) {
            std::cerr << "Error opening file.\n";
            return;
        }

        passwords_file.open(path, std::ios::in);
        while (std::getline(passwords_file, line)) { 
            std::cout << line << std::endl;
        }
    passwords_file.close();


    }


    std::vector<std::string> find_password(std::string name) {
        std::vector<std::string> res;
        std::ifstream passwords_file;
        std::string line;
        if (!passwords_file.is_open()) {
            std::cerr << "Error opening file.\n";
            return res;
        }

        passwords_file.open(path, std::ios::in);
        while (std::getline(passwords_file, line)) {
            if (line.substr(0, line.find(' ')) == name)
                res.push_back(line);
            }
        passwords_file.close();

        return res;
    }

    void delete_password(std::string line_to_delete) {
        std::ofstream file_o(temp_filename);
        std::ifstream file_i(path);

        if (!file_i.is_open() || !file_o.is_open()) {
            std::cerr << "Error opening file.\n";
            return;
        }

        std::string line;
        while (std::getline(file_i, line)) {
            if (line != line_to_delete) {
                file_o << line << '\n';
            }
        }
        file_i.close();
        file_o.close();

        std::remove(path.c_str());
        std::rename(temp_filename.c_str(), path.c_str());
        std::cout << "Password has been deleted.\n";
    }

};


void show_menu() {
    std::cout << "========== Terminal Password Manager ==========\n"
            << "1. Add password\n"
            << "2. Show all services\n"
            << "3. Find password\n"
            << "4. Delete password\n"
            << "5. Exit\n";
}

int main() {
    PasswordManager manager;
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
            std::vector<std::string> found_passwords = manager.find_password(service_name);
            if (!found_passwords.empty()) {
                std::cout << "============== Founded passwords ==============\n";
                for (const std::string &s : found_passwords) {
                    std::cout << s << '\n';
                }
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