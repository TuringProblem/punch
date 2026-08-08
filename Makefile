
SOURCES != find ./src -name "*.cpp"

main: $(SOURCES)
	clang++22 $(SOURCES) -std=c++23 -o ./build/main

run: main
	./build/main message
