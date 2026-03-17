# Terminal Password Manager C++

## Build

Install dependencies (Ubuntu / Debian):
sudo apt install build-essential libsqlite3-dev

Using Makefile:
make run

or

Compile (gcc):

g++ src/main.cpp -std=c++20 -o manager -lsqlite3

Run:
./manager