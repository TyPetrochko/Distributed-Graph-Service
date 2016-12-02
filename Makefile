CC = g++
CFLAGS = -std=c++11 -pedantic -Wall -I ./include -g -lthrift

all: main

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o $@

main: main.cpp
	${CC} ${CFLAGS} $^ memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp -L /usr/local/lib/ -lthrift -std=c++0x -pthread -o cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) cs426_graph_server memorygraph

run: main
	./cs426_graph_server 8080

test:
	/c/cs426/scripts/lab1test.sh http://127.0.0.1:8080
