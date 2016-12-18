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

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace rpc;
using namespace std;

void serve_partition(int port, int partition);

vector<GraphEditClient *> clients;

char *ip;

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
        if(!part_add_node(p.node_a))
          ret = false;
        else
          add_node(p.node_a);
        break;
      case Operation::ADD_EDGE:
        if(!part_add_edge(p.node_a, p.node_b))
          ret = false;
        else
          add_edge(p.node_a, p.node_b);
        break;
      case Operation::REMOVE_NODE:
        if(!part_remove_node(p.node_a))
          ret = false;
        else
          remove_node(p.node_a);
        break;
      case Operation::REMOVE_EDGE:
        if(!part_remove_edge(p.node_a, p.node_b))
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

void partitioning_init(int partition, vector<char*> partitions){
  string us = string(partitions[partition - 1]);
  int local_port = atoi(us.substr(1 + us.find(":")).c_str());
  thread service(serve_partition, local_port, partition);
  service.detach();

  unsigned int i;
  for (i = 0; i < partitions.size(); i++){
    string p = string(partitions[i]);
      
    if(DEBUG)
      cout << "Connecting to partition " << i + 1 << ", " << p << "..."
        << endl;

    string ip_adr = p.substr(0, p.find(":"));
    int port = atoi(p.substr(1 + p.find(":")).c_str());
    
    boost::shared_ptr<TTransport> socket(new TSocket(ip_adr, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    clients.push_back(new GraphEditClient(protocol));
  
    // wait for all partitions to come on-line
    while(true){
      try {
        transport->open();
        if(DEBUG)
          cout << "\tConnected to partition " << i + 1 << ", " << p << "!" 
            << endl;
        break;
      } catch(TException& tx) {
        if(DEBUG)
          cout << "." << endl;
        sleep(1);
      }
    }
  }

  if(DEBUG)
    cout << "Connected to all partitions!" << endl;
}

void serve_partition(int port, int partition){
  if(DEBUG)
    cout << "Partition server " << partition << " starting up on port "
      << port << endl;
  
  boost::shared_ptr<GraphEditHandler> handler(new GraphEditHandler());
  boost::shared_ptr<TProcessor> processor(new GraphEditProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  
  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
 
  // DIFFERENT SERVER-TYPE
  // TThreadedServer server(
  //   boost::make_shared<GraphEditProcessorFactory>(boost::make_shared<GraphEditCloneFactory>()),
  //   boost::make_shared<TServerSocket>(port),
  //   boost::make_shared<TBufferedTransportFactory>(),
  //   boost::make_shared<TBinaryProtocolFactory>());
  // server.serve();
  cerr << "Aborting!" << endl;
}

bool part_add_node(int64_t node_id){
  // Packet p;
  // p.op = Operation::ADD_NODE;
  // p.node_a = node_id;
  // if(replicating){
  //   try {
  //     return client->editGraph(p);
  //   } catch(TException& tx) {
  //     if(DEBUG)
  //       cout << "REPLICATION ERROR: " << tx.what() << endl;
  //     return false;
  //   }
  // }
  return true;
}
bool part_add_edge(int64_t node_a_id, int64_t node_b_id){
  // Packet p;
  // p.op = Operation::ADD_EDGE;
  // p.node_a = node_a_id;
  // p.node_b = node_b_id;
  // if(replicating){
  //   try {
  //     return client->editGraph(p);
  //   } catch(TException& tx) {
  //     if(DEBUG)
  //       cout << "REPLICATION ERROR: " << tx.what() << endl;
  //     return false;
  //   }
  // }
  return true;
}
bool part_remove_node(int64_t node_id){
  // Packet p;
  // p.op = Operation::REMOVE_NODE;
  // p.node_a = node_id;
  // if(replicating){
  //   try {
  //     return client->editGraph(p);
  //   } catch(TException& tx) {
  //     if(DEBUG)
  //       cout << "REPLICATION ERROR: " << tx.what() << endl;
  //     return false;
  //   }
  // }
  return true;
}
bool part_remove_edge(int64_t node_a_id, int64_t node_b_id){
  // Packet p;
  // p.op = Operation::REMOVE_EDGE;
  // p.node_a = node_a_id;
  // p.node_b = node_b_id;
  // if(replicating){
  //   try {
  //     return client->editGraph(p);
  //   } catch(TException& tx) {
  //     if(DEBUG)
  //       cout << "REPLICATION ERROR: " << tx.what() << endl;
  //     return false;
  //   }
  // }
  return true;
}

