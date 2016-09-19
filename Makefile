CC = g++
CFLAGS = -std=c++11 -pedantic -Wall -I ./include -g

all: memorygraph main

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o build/$@

main: main.cpp
	${CC} ${CFLAGS} $^ memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp -o build/cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) build/*

run: main
	build/cs426_graph_server 8080
