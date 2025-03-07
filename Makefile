# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -I    include path
#  -m64  64-bit architecture, needed when running in WSL
CFLAGS  = -g -Wall -Wextra -std=c++17 -fopenmp -Idatamodel -m64  

# Google Test configuration
GTEST_LIBS = -lgtest -lgtest_main -pthread
GTEST_INCLUDE = -I/usr/include/gtest

# the build target executable:
TARGET = main
TEST_TARGET = test_runner

# Source files
SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
OBJECTS = $(patsubst %.cpp, ./build/%.o, $(SOURCES))

# Test source files (adjust path as needed)
TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, ./build/%.o, $(TEST_SOURCES))

$(info Src dependency objects: $(OBJECTS))
$(info Test dependency objects: $(TEST_OBJECTS))

# Compile object files
./build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Main executable
$(TARGET): $(TARGET).cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(TARGET).cpp -o ./build/$(TARGET).exe

# Test executable
$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(GTEST_INCLUDE) $^ $(GTEST_LIBS) -o ./build/$(TEST_TARGET).exe

all: $(TARGET) $(TEST_TARGET)
tests: $(TEST_TARGET)
server: $(TARGET)

run:
	./build/$(TARGET).exe

test: $(TEST_TARGET)
	./build/$(TEST_TARGET).exe

clean:
	rm -rf ./build/*