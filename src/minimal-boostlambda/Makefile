CC = g++
CXX_FLAGS = -O2 -std=c++11 -Wall -Wextra

all: main

main: main.o
	$(CC) $^ -o $@ $(CXX_FLAGS)

main.o: main.cpp lambda.h
	$(CC) $< -c -o $@ $(CXX_FLAGS)

.PHONY: clean

clean:
	rm main.o main -f
