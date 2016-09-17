CC = g++
CFLAGS = -std=c++11 -Wall -pedantic

all: memorygraph

memorygraph: memorygraph.o
	${CC} ${CFLAGS} -o $@ $^

main: main.cpp
	${CC} ${CFLAGS} $^ -o cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) memorygraph cs426_graph_server *.o
