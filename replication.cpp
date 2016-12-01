#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>

#include "memorygraph.hpp"
#include "gen-cpp/GraphEdit.h"

#define DEBUG (false)

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::rpc;

void serve_replica();

char *ip;

class GraphEditHandler : virtual public GraphEditIf {
 public:
  GraphEditHandler() {
    // Your initialization goes here
  }

  bool editGraph(const Packet& p) {
    if(DEBUG)
      cout << "Server received a request!" << endl;

    switch(p.op){
      case ADD_NODE:
      case ADD_EDGE:
      case REMOVE_NODE:
      case REMOVE_EDGE:
      default:
        // TODO
        cout << "Server does not handle requests properly yet!" << endl;
    }
  }

};

void replica_init(char *ip_adr){
  if(DEBUG)
    cout << "Replicating from ip addr " << ip_adr << endl;
  
  ip = ip_adr;
 
  thread service(serve_replica);
}

void serve_replica(){
  if(DEBUG)
    cout << "Replica service thread starting up" << endl;
  
  int port = 9090;
  shared_ptr<GraphEditHandler> handler(new GraphEditHandler());
  shared_ptr<TProcessor> processor(new GraphEditProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
}

