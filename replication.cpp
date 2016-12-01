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

#define DEBUG (false)

using namespace std;

void serve_replica();

char *ip;

void replica_init(char *ip_adr){
  if(DEBUG)
    cout << "Replicating from ip addr " << ip_adr << endl;
  
  ip = ip_adr;
 
  thread service(serve_replica);
}

void serve_replica(){
  if(DEBUG)
    cout << "Replica service thread starting up" << endl;

}

