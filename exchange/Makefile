# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -I    include path
#  -m64  64-bit architecture, needed when running in WSL
CFLAGS  = -g -Wall -Wextra -std=c++17 -fopenmp -Idatamodel -m64 -pthread

# Google Test configuration
GTEST_LIBS = -lgtest -lgtest_main # -pthread <- already included in CFLAGS
GTEST_INCLUDE = -I/usr/include/gtest

# the build target executable:
TARGET = main
TEST_TARGET = test_runner
SERVER_TARGET = exchange
CLIENT_TARGET = test_client

# Source files
SOURCES = $(wildcard datamodel/*.cpp) \
		  $(wildcard src/*.cpp) \
		  $(wildcard src/**/*.cpp) \
		  $(wildcard src/**/**/*.cpp)
OBJECTS = $(patsubst %.cpp, ./build/%.o, $(SOURCES))

# Test source files (adjust path as needed)
TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, ./build/%.o, $(TEST_SOURCES))

# Client test source file
CLIENT_SOURCE = tests/tcp_test_client.cpp
CLIENT_OBJECT = ./build/$(CLIENT_SOURCE:.cpp=.o)

$(info Src dependency objects: $(OBJECTS))
$(info Test dependency objects: $(TEST_OBJECTS))

# Compile object files
./build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(TARGET).cpp -o ./build/$(TARGET).exe

# Test executable
$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(GTEST_INCLUDE) $^ $(GTEST_LIBS) -o ./build/$(TEST_TARGET).exe

# # Server executable
# server: $(OBJECTS)
# 	$(CC) $(CFLAGS) $(OBJECTS) -o ./build/$(SERVER_TARGET).exe

# # Client test executable
# client: $(OBJECTS)
# 	@mkdir -p $(dir ./build/$(CLIENT_SOURCE))
# 	$(CC) $(CFLAGS) $(CLIENT_SOURCE) -o ./build/$(CLIENT_TARGET).exe

all: clean $(TARGET) $(TEST_TARGET)

run:
	./build/$(TARGET).exe

test:
	./build/$(TEST_TARGET).exe

# run_server:
# 	./build/$(SERVER_TARGET).exe

# run_client:
# 	./build/$(CLIENT_TARGET).exe

clean:
	rm -rf ./build/*