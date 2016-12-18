CC = g++
CFLAGS = -std=c++11 -pedantic -Wall -I ./include -g

all: main

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o $@

main: main.cpp
	${CC} ${CFLAGS} $^ replication.cpp memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp gen-cpp/GraphEdit.cpp gen-cpp/rpc_constants.cpp gen-cpp/rpc_types.cpp -g -lpthread -lthrift -o cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) cs426_graph_server memorygraph

run1:
	./cs426_graph_server 8000 -p 1 -l 104.196.163.178:6000 104.196.163.178:6001

run2:
	./cs426_graph_server 8001 -p 2 -l 104.196.163.178:6000 104.196.163.178:6001

test:
	/c/cs426/scripts/lab1test.sh http://127.0.0.1:8080
