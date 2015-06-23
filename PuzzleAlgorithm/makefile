CC = g++

all: puzzle

puzzle: state.cpp main.cpp
	$(CC) -o puzzle state.cpp main.cpp double.cpp -lpthread

debug: state.cpp main.cpp
	$(CC) -o puzzle main.cpp state.cpp double.cpp -lpthread -g

