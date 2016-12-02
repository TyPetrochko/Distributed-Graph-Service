CC = g++
CFLAGS = -std=c++11 -pedantic -Wall -I ./include -g

all: main

memorygraph: memorygraph.cpp memorygraph.hpp
	${CC} ${CFLAGS} memorygraph.cpp -o $@

main: main.cpp
	${CC} ${CFLAGS} $^ memorygraph.cpp include/mongoose.c include/JSON.cpp include/JSONValue.cpp gen-cpp/GraphEdit.cpp gen-cpp/rpc_constants.cpp gen-cpp/rpc_types.cpp -L/usr/local/lib `pkg-config --libs grpc++ grpc` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -lprotobuf -lpthread -ldl -std=c++0x -pthread -o cs426_graph_server

memorygraph.o: memorygraph.cpp memorygraph.hpp

clean:
	$(RM) cs426_graph_server memorygraph

run: main
	./cs426_graph_server 8080

test:
	/c/cs426/scripts/lab1test.sh http://127.0.0.1:8080
