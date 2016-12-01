#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "GraphEdit.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace rpc;

int main() {
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  GraphEditClient client(protocol);

  try {
    transport->open();

    Packet p;
    p.op = Operation::ADD_NODE;
    p.node_a = 1;

    client.editGraph(p);

    transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}