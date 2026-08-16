
SOURCES := $(shell find ./src -name "*.cpp")
SOURCE_BSD !=  find ./src -name "*.cpp"

main: $(SOURCES)
	clang++ $(SOURCES) -std=c++23 -o ./build/main

run: main
	./build/main message

bsd:
	clang++22 $(SOURCE_BSD) -std=c++23 -o ./build/main
