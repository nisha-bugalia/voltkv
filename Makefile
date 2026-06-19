# Define the compiler we want to use
CXX = g++

# Define the compiler flags (C++ standard version and warnings)
CXXFLAGS = -std=c++17 -Wall

# Define the final executable output name
TARGET = voltkv.exe

# The recipe to build our project
all:
	$(CXX) $(CXXFLAGS) src/main.cpp src/parser/parser.cpp src/engines/engine.cpp -o $(TARGET)

# A helper recipe to clean up old executable files
clean:
	del $(TARGET)