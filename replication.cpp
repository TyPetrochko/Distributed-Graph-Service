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
#include <thrift/transport/TBufferTransports.h>

#include "replication.hpp"
#include "memorygraph.hpp"
#include "gen-cpp/GraphEdit.h"

#define DEBUG (true)
#define PORT (9090)

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace rpc;
using namespace std;

void serve_replica();

GraphEditClient *client;

char *ip;
bool replicating = false;

class GraphEditHandler : virtual public GraphEditIf {
 public:
  GraphEditHandler() {
    // no initialization code yet
  }

  bool editGraph(const Packet& p) {
    int ret;
    if(DEBUG)
      cout << "Server received a request!" << endl;
   
    // replicate if necessary, then perform locally, then return
    ret = true;
    switch(p.op) {
      case Operation::ADD_NODE:
        if(!repl_add_node(p.node_a))
          ret = false;
        else
          add_node(p.node_a);
        break;
      case Operation::ADD_EDGE:
        if(!repl_add_edge(p.node_a, p.node_b))
          ret = false;
        else
          add_edge(p.node_a, p.node_b);
        break;
      case Operation::REMOVE_NODE:
        if(!repl_remove_node(p.node_a))
          ret = false;
        else
          remove_node(p.node_a);
        break;
      case Operation::REMOVE_EDGE:
        if(!repl_remove_edge(p.node_a, p.node_b))
          ret = false;
        else
          remove_edge(p.node_a, p.node_b);
        break;
      default:
        cerr << "Received bad operation: " << p.op << endl;
      	ret = false;
        break;
    }
    return ret;
  }
};

void replica_init(){
  thread service(serve_replica);
  service.detach();
}

void master_init(char *ip_adr){
  if(DEBUG)
    cout << "Replicating on ip " << ip_adr << ", port " << PORT << endl;
  replicating = true;

  boost::shared_ptr<TTransport> socket(new TSocket(ip_adr, PORT));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  client = new GraphEditClient(protocol);

  try {
    transport->open();
  } catch(TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void serve_replica(){
  if(DEBUG)
    cout << "Replica service thread starting up on port " << PORT << endl;
  
  boost::shared_ptr<GraphEditHandler> handler(new GraphEditHandler());
  boost::shared_ptr<TProcessor> processor(new GraphEditProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(PORT));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  
  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  
  // TThreadedServer server(
  //   boost::make_shared<GraphEditProcessorFactory>(boost::make_shared<GraphEditCloneFactory>()),
  //   boost::make_shared<TServerSocket>(PORT),
  //   boost::make_shared<TBufferedTransportFactory>(),
  //   boost::make_shared<TBinaryProtocolFactory>());
  // server.serve();
  cerr << "Aborting!" << endl;
}

bool repl_add_node(int64_t node_id){
  Packet p;
  p.op = Operation::ADD_NODE;
  p.node_a = node_id;
  if(replicating){
    try {
      return client->editGraph(p);
    } catch(TException& tx) {
      if(DEBUG)
        cout << "REPLICATION ERROR: " << tx.what() << endl;
      return false;
    }
  }
  return true;
}
bool repl_add_edge(int64_t node_a_id, int64_t node_b_id){
  Packet p;
  p.op = Operation::ADD_EDGE;
  p.node_a = node_a_id;
  p.node_b = node_b_id;
  if(replicating){
    try {
      return client->editGraph(p);
    } catch(TException& tx) {
      if(DEBUG)
        cout << "REPLICATION ERROR: " << tx.what() << endl;
      return false;
    }
  }
  return true;
}
bool repl_remove_node(int64_t node_id){
  Packet p;
  p.op = Operation::REMOVE_NODE;
  p.node_a = node_id;
  if(replicating){
    try {
      return client->editGraph(p);
    } catch(TException& tx) {
      if(DEBUG)
        cout << "REPLICATION ERROR: " << tx.what() << endl;
      return false;
    }
  }
  return true;
}
bool repl_remove_edge(int64_t node_a_id, int64_t node_b_id){
  Packet p;
  p.op = Operation::REMOVE_EDGE;
  p.node_a = node_a_id;
  p.node_b = node_b_id;
  if(replicating){
    try {
      return client->editGraph(p);
    } catch(TException& tx) {
      if(DEBUG)
        cout << "REPLICATION ERROR: " << tx.what() << endl;
      return false;
    }
  }
  return true;
}

