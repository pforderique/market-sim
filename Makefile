# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -I    include path
#  -m64  64-bit architecture, needed when running in WSL
CFLAGS  = -g -Wall -Wextra -std=c++17 -fopenmp -Idatamodel -m64  

# the build target executable:
TARGET = main
SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
OBJECTS = $(patsubst %.cpp, ./build/%.o, $(SOURCES))

$(info Dependency objects: $(OBJECTS))

./build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(TARGET).cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(TARGET).cpp -o ./build/$(TARGET).exe

all: $(TARGET) # clean # removed clean to keep the executable file

run:
	./build/$(TARGET).exe

clean:
	rm -rf ./build/*