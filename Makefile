CC = g++
CFLAGS = -std=c++11 -pedantic -Wall -I ./include -g

all: main

cross_platform:
	${CC} -std=c++11 -I ./include -g ${CFLAGS} $^ util.cpp persistence.cpp memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp -o cs426_graph_server

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o $@

main: main.cpp
	${CC} ${CFLAGS} $^ util.cpp persistence.cpp memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp -o cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) cs426_graph_server memorygraph

run:
	./cs426_graph_server 8080 dev_file

test:
	/c/cs426/scripts/lab1test.sh http://127.0.0.1:8080
