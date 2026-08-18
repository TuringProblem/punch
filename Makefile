
SOURCES := $(shell find ./src -name "*.cpp")
SOURCE_BSD !=  find ./src -name "*.cpp"

# Header search paths for #include "..." lookups (see docs/INCLUDE_PATHS.md)
INCLUDES := -I./include -I./src

main: $(SOURCES)
	clang++ $(SOURCES) $(INCLUDES) -std=c++23 -o ./build/main

run: main
	./build/main message

bsd:
	clang++22 $(SOURCE_BSD) $(INCLUDES) -std=c++23 -o ./build/main
