CC = g++
CXXFLAGS = -std=c++17 -lGL -lglfw -lGLEW -lnanogui 

all: main

debug: CXXFLAGS += -DDEBUG -g
debug: main

main: main.o Renderer.o Curve.o Aircraft_Animation.o
	$(CC) $(CXXFLAGS) build/main.o build/Renderer.o build/Curve.o build/Aircraft_Animation.o -o main

main.o: main.cpp Renderer.h
	mkdir -p build
	$(CC) $(CXXFLAGS) -c main.cpp -o build/main.o

Renderer.o: Renderer.cpp Renderer.h
	$(CC) $(CXXFLAGS) -c Renderer.cpp -o build/Renderer.o

Curve.o: Curve.cpp Curve.h
	$(CC) $(CXXFLAGS) -c Curve.cpp -o build/Curve.o

Aircraft_Animation.o: Aircraft_Animation.cpp Aircraft_Animation.h
	$(CC) $(CXXFLAGS) -c Aircraft_Animation.cpp -o build/Aircraft_Animation.o

.PHONY: clean
clean:
	rm -rf build
	rm -r main