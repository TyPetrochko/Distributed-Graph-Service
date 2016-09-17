CC = g++
CFLAGS = -std=c++11 -Wall -pedantic -I ./include

all: memorygraph main

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o build/$@

main: main.cpp
	${CC} ${CFLAGS} $^ -o build/cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) build/*
