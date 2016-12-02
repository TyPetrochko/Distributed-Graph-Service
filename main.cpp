/* Much of this code is adapted from the mongoose examples page,
 * https://docs.cesanta.com/mongoose/dev/#/usage-example/ */
#define DEBUG (false)

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "mongoose.h"
#include "memorygraph.hpp"
#include "JSON.h"
#include "replication.hpp"

#include "gen-cpp/GraphEdit.h"

using namespace std;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using boost::shared_ptr;

using namespace rpc;

class GraphEditHandler : virtual public GraphEditIf {
 public:
 	char *nextip = 0;
 	char *nextport = 0;

  GraphEditHandler(char *ip, char *port) {
    // Your initialization goes here
    nextip = ip;
    nextport = port;
  }

  bool editGraph(const Packet& p) {
    // Your implementation goes here
    if(nextip != 0 && nextport != 0) {
	    boost::shared_ptr<TTransport> socket(new TSocket(nextip, stoi(nextport)));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		GraphEditClient client(protocol);
		bool ret;
		try {
			transport->open();
			ret = client.editGraph(p);
			transport->close();
			if (!ret) {
				return ret;
			}
		} catch (TException& tx) {
			cout << "ERROR: " << tx.what() << endl;
			return false;
		}
	}

    ret = true;
    switch(p.op) {
      case Operation::ADD_NODE:
        add_node(p.node_a);
        break;
      case Operation::ADD_EDGE:
        add_edge(p.node_a, p.node_b);
        break;
      case Operation::REMOVE_NODE:
        remove_add(p.node_a);
        break;
      case Operation::REMOVE_EDGE:
        remove_edge(p.node_a, p.node_b);
        break;
      default:
      	ret = false;
        break;
    }
    return ret;
  }
};

// default error message
string err_msg = string("HTTP/1.1 400 Bad Request\r\n")
+ "Content-Length: 0\r\n"
+	"Content-Type: application/json\r\n";

// are we a slave (replica) and ip addr if so
bool slave = false;
char *ip_addr = 0;
char *port = "9090";

void process_args(int argc, char **argv) {
	if (argc < 2 || argc > 4){
    cerr << "Usage: ./cs426_graph_server [-p <ip_addr>] <port>" << endl;
		exit(1);
	}

	int option_char;
  while ((option_char = getopt(argc, argv, "b")) != -1) {
    switch (option_char){
      case 'b':
        slave = true;
        ip_addr = optarg;
        break;
      default: 
        cerr << "Usage: ./cs426_graph_server [-p <ip_addr>] <port>" << endl;
        exit(1);
    }
  }
  for(int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      continue;
    }

    if (port == 0) {
      port = argv[i];
    }
  }
}

// convert mongoose's weird string struct to a regular c++ string
string mg_str_to_string(struct mg_str m){
	string s = string(m.p).substr(0, m.len);
	return s;
}

// convert a json object to a c++ string (supports unicode)
string json_to_string(JSONObject j){
	wstring ws((new JSONValue(j))->Stringify());
	string str(ws.begin(), ws.end());
	return str;
}

// handle an incoming api call
string handle_request(string body, string uri){
	JSONValue *value;
	JSONObject root;
	string func;
	string prefix = "/api/v1/";

	// parse any json in the request
	value = JSON::Parse(body.c_str());
	if(value == NULL || value->IsObject() == false){
		return err_msg;
	}
	root = value->AsObject();

	// remove prefix from uri
	func = uri;
	if(func.length() <= prefix.length()){
		return err_msg;
	}
	func.erase(0, prefix.length());

	// initialize default values for response
	string payload  = "";
	string proto = "HTTP/1.1";
	int resp_code = 400;

	/*
	 * First propagate the request to the next server before committing own
	 */
	if(ip_addr != 0) {
		boost::shared_ptr<TTransport> socket(new TSocket(ip_addr, stoi(port)));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		GraphEditClient client(protocol);

		try {
			transport->open();

			bool write = false;
			Packet p;
			if(func == "add_node" && root[L"node_id"] && root[L"node_id"]->IsNumber()){
				p.op = Operation::ADD_NODE;
				p.node_a = root[L"node_id"];
				write = true;
			} else if (func == "add_edge" 
				&& root[L"node_a_id"]
				&& root[L"node_b_id"]
				&& root[L"node_a_id"]->IsNumber() 
				&& root[L"node_b_id"]->IsNumber()){
				p.op = Operation::ADD_EDGE;
				p.node_a = root[L"node_a_id"];
				p.node_b = root[L"node_b_id"];
				write = true;
			} else if (func == "remove_node" 
					&& root[L"node_id"] 
					&& root[L"node_id"]->IsNumber()){
				p.op = Operation::REMOVE_NODE;
				p.node_a = root[L"node_id"];
				write = true;
			}else if (func == "remove_edge"
					&& root[L"node_a_id"]
					&& root[L"node_b_id"]
					&& root[L"node_a_id"]->IsNumber() 
					&& root[L"node_b_id"]->IsNumber()){
				p.op = Operation::REMOVE_EDGE;
				p.node_a = root[L"node_a_id"];
				p.node_b = root[L"node_b_id"];
				write = true;
			}

			if (write) {
				bool ret = client.editGraph(p);

				transport->close();
				if (!ret) {
					// give appropriate status message
					string resp_status_msg = "Bad Request";

					// build header
					string headers_string = "Content-Length: " 
						+ to_string(0)
						+ "\r\n"
						+ "Content-Type: application/json";

					// build text to send
					string to_send = proto
						+ " "
						+ to_string(500)
						+ " "
						+ resp_status_msg
						+ "\r\n"
						+ headers_string;
					
					to_send += "\r\n";

					return to_send;
				}
			}
		} catch (TException& tx) {
			cout << "ERROR: " << tx.what() << endl;
			return err_msg;
		}
	}

	// giant pseudo-switch statement to actually call the API
	if(func == "add_node" && root[L"node_id"] && root[L"node_id"]->IsNumber()){
		resp_code = add_node(root[L"node_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	}else if (func == "add_edge" 
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		resp_code = add_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	}else if (func == "remove_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		resp_code = remove_node(root[L"node_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	}else if (func == "remove_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		resp_code = remove_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	}else if (func == "get_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		struct nodeData d = get_node(root[L"node_id"]->AsNumber());
		resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		payload = json_to_string(return_data).c_str();
	}else if (func == "get_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		struct nodeData d = get_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		payload = json_to_string(return_data).c_str();
	}else if (func == "get_neighbors" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		struct neighborData d = get_neighbors(root[L"node_id"]->AsNumber());
		if((resp_code = d.status) == 200){
			JSONObject return_data;
			JSONArray neighbors;
			for(int64_t n : d.neighbors){
				neighbors.push_back(new JSONValue((double) n));
			}
			return_data[L"node_id"] = root[L"node_id"];
			return_data[L"neighbors"] = new JSONValue(neighbors);
			payload = json_to_string(return_data);
		}
	}else if (func == "shortest_path"
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		struct distanceData d = shortest_path(
				root[L"node_a_id"]->AsNumber(), 
				root[L"node_b_id"]->AsNumber());
		if((resp_code = d.status) == 200){
			JSONObject return_data;
			return_data[L"distance"] = new JSONValue((int) d.distance);
			payload = json_to_string(return_data).c_str();
		}
	} else{
		return err_msg;
	}

	// give appropriate status message
	string resp_status_msg;
	if(resp_code == 400)
		resp_status_msg = "Bad Request";
	else
		resp_status_msg = "OK";

	// build header
	string headers_string = "Content-Length: " 
		+ to_string(payload.length())
		+ "\r\n"
		+ "Content-Type: application/json";

	// build text to send
	string to_send = proto
		+ " "
		+ to_string(resp_code)
		+ " "
		+ resp_status_msg
		+ "\r\n"
		+ headers_string;
	if(payload != "")
		to_send = to_send + "\r\n\r\n" + payload;
	
	to_send += "\r\n";

	return to_send;
}

// event handler
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	struct mbuf *io = &nc->recv_mbuf;
	switch (ev) {
		case MG_EV_HTTP_REQUEST:
			{
				string to_send = handle_request(
						mg_str_to_string(hm->body), mg_str_to_string(hm->uri));
				
				//send it
				mg_send(nc, to_send.c_str(), to_send.length());

				// debug mode!
				if(DEBUG){
					cout << "Response sent:" << endl;
					cout << to_send << endl;
				}

				// close the connection!
				nc->flags |= MG_F_SEND_AND_CLOSE;
				mbuf_remove(io, io->len);
			}
			break;
    default:
      break;
  }
}

int main(int argc, char *argv[]){
	process_args(argc, argv);

	/*
	 * Server thrift code for replication
	 */
	shared_ptr<GraphEditHandler> handler(new GraphEditHandler(ip_addr));
  	shared_ptr<TProcessor> processor(new GraphEditProcessor(handler));
  	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();

  if(slave)
    replica_init(ip_addr);

	struct mg_connection *nc;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);  // Initialize event manager object

  // Note that many connections can be added to a single event manager
  // Connections can be created at any point, e.g. in event handler function
  nc = mg_bind(&mgr, port, ev_handler);
	
	// listen for http
	mg_set_protocol_http_websocket(nc);

	for (;;) {  // Start infinite event loop
    mg_mgr_poll(&mgr, 1000);
  }

  mg_mgr_free(&mgr);
  return 0;
}
