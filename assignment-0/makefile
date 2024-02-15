CC = g++
CXXFLAGS = -std=c++17 -lGL -lglfw -lGLEW -lnanogui 

all: main

debug: CXXFLAGS += -DDEBUG -g
debug: main

main: main.cpp shader.h
	$(CC) $(CXXFLAGS) main.cpp -o main

.PHONY: clean
clean:
	rm main
