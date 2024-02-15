CC = g++
CXXFLAGS = -std=c++17 -lGL -lglfw -lGLEW -lnanogui 

all: main

debug: CXXFLAGS += -DDEBUG -g
debug: main

main: main.o Renderer.o Bone_Animation.o
	$(CC) $(CXXFLAGS) build/main.o build/Renderer.o build/Bone_Animation.o -o main

main.o: main.cpp Renderer.h
	mkdir -p build
	$(CC) $(CXXFLAGS) -c main.cpp -o build/main.o

Renderer.o: Renderer.cpp Renderer.h
	$(CC) $(CXXFLAGS) -c Renderer.cpp -o build/Renderer.o

Bone_Animation.o: Bone_Animation.cpp Bone_Animation.h
	$(CC) $(CXXFLAGS) -c Bone_Animation.cpp -o build/Bone_Animation.o

.PHONY: clean
clean:
	rm -rf build
	rm -r main