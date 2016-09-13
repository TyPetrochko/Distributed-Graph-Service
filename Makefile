CC = g++
CFLAGS = -std=c++11 -Wall -pedantic

all: memorygraph

memorygraph: memorygraph.o
	${CC} ${CFLAGS} -o $@ $^

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) memorygraph *.o