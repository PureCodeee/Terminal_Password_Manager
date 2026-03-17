CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lsqlite3

SRC = src/main.cpp
OUT = build/manager

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LIBS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)