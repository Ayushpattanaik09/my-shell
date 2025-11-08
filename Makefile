CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC = src/main.cpp src/parser.cpp src/io.cpp src/jobs.cpp
all: myshell
myshell: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o myshell
clean:
	rm -f myshell
