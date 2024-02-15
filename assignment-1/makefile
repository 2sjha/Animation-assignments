CC = g++
CXXFLAGS = -std=c++17 -lGL -lglfw -lGLEW -lnanogui 

all: main

debug: CXXFLAGS += -DDEBUG -g
debug: main

main: main.o Renderer.o Animation.o
	$(CC) $(CXXFLAGS) build/main.o build/Renderer.o build/Animation.o -o main

main.o: main.cpp Renderer.h
	mkdir -p build
	$(CC) $(CXXFLAGS) -c main.cpp -o build/main.o

Renderer.o: Renderer.cpp Renderer.h
	$(CC) $(CXXFLAGS) -c Renderer.cpp -o build/Renderer.o

Animation.o: Animation.cpp Animation.h
	$(CC) $(CXXFLAGS) -c Animation.cpp -o build/Animation.o

.PHONY: clean
clean:
	rm -rf build
	rm -r main