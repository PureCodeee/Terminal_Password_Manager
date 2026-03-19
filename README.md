# Terminal Password Manager C++

## Build

Install dependencies (Ubuntu / Debian):

sudo apt install build-essential libsqlite3-dev

Using Makefile:

make run

or

Compile (gcc):

g++ -std=c++17 -Wall -Iinclude src/main.cpp src/database.cpp src/password_manager.cpp -o build/manager -lsqlite3

Run:

./build/manager