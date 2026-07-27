#Build and Run Instructions:

Option A — CMake
mkdir build && cd build && cmake .. && make
./stocksim

Option B — Direct g++
g++ -std=c++11 -Iinclude src/*.cpp main.cpp -o stocksim
./stocksim
