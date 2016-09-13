CC = g++
CFLAGS = -std=c++11 -Wall -pedantic

all: memorygraph

memorygraph.o: memorygraph.cpp memorygraph.hpp

memorygraph: memorygraph.o
	${CC} ${CFLAGS} -o $@ $^

clean:
	$(RM) memorygraph *.o