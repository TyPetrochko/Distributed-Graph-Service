/* Much of this code is adapted from the mongoose examples page,
 * https://docs.cesanta.com/mongoose/dev/#/usage-example/ */
#define DEBUG (false)

#include <iostream>
#include <vector>
#include <string>

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

#include <boost/make_shared.hpp>

#include "mongoose.h"
#include "memorygraph.hpp"
#include "JSON.h"
#include "replication.hpp"

#include "gen-cpp/GraphEdit.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace rpc;
using namespace std;

// default error message
string err_msg = string("HTTP/1.1 400 Bad Request\r\n")
+ "Content-Length: 0\r\n"
+	"Content-Type: application/json\r\n";

bool master = false;
char *ip_addr = 0;
char *port = 0;

void process_args(int argc, char **argv) {
	if (argc < 2 || argc > 4){
    cerr << "Usage: ./cs426_graph_server [-b <ip_addr>] <port>" << endl;
		exit(1);
	}

	int option_char;
  while ((option_char = getopt(argc, argv, "b:")) != -1) {
    switch (option_char){
      case 'b':
        if(optarg == NULL)
          cerr << "WTF OPTARG NULL" << endl;
        master = true;
        ip_addr = optarg;
        break;
      default: 
        cerr << "Usage: ./cs426_graph_server [-b <ip_addr>] <port>" << endl;
        exit(1);
    }
  }
  for(int i = 1; i < argc; i++) {
    if (argv[i][0] == '-')
      continue;
    else if(ip_addr && !strcmp(argv[i], ip_addr))
      continue;
    
    if(port == 0)
      port = argv[i];
  }

  if(port == 0)
    cerr << "No port specified!" << endl;
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

  if(DEBUG)
    cout << "REQUEST to " << uri << ": " << body << endl;

  // parse any json in the request
  value = JSON::Parse(body.c_str());
  if(value == NULL || value->IsObject() == false){
    if(DEBUG)
      cout << "Bad json! " <<  endl;
    return err_msg;
  }
  root = value->AsObject();

  // remove prefix from uri
  func = uri;
  if(func.length() <= prefix.length()){
    if(DEBUG)
      cout << "Bad function: " << uri  << endl;
    return err_msg;
  }
  func.erase(0, prefix.length());

  // initialize default values for response
  string payload  = "";
  string proto = "HTTP/1.1";
  int resp_code = 400;

  // giant pseudo-switch statement to actually call the API
  if(func == "add_node" && root[L"node_id"] && root[L"node_id"]->IsNumber()){
    if(!repl_add_node(root[L"node_id"]->AsNumber()))
      resp_code = 500;
    else
      resp_code = add_node(root[L"node_id"]->AsNumber());
    if(resp_code == 200) payload = body;
  }else if (func == "add_edge" 
      && root[L"node_a_id"]
      && root[L"node_b_id"]
      && root[L"node_a_id"]->IsNumber() 
      && root[L"node_b_id"]->IsNumber()){
    if(!repl_add_edge(
          root[L"node_a_id"]->AsNumber(),
          root[L"node_b_id"]->AsNumber()))
      resp_code = 500;
    else
      resp_code = add_edge(
          root[L"node_a_id"]->AsNumber(),
          root[L"node_b_id"]->AsNumber());
    if(resp_code == 200) payload = body;
  }else if (func == "remove_node" 
      && root[L"node_id"] 
      && root[L"node_id"]->IsNumber()){
    if(!repl_remove_node(root[L"node_id"]->AsNumber()))
      resp_code = 500;
    else
      resp_code = remove_node(root[L"node_id"]->AsNumber());
    if(resp_code == 200) payload = body;
  }else if (func == "remove_edge"
      && root[L"node_a_id"]
      && root[L"node_b_id"]
      && root[L"node_a_id"]->IsNumber() 
      && root[L"node_b_id"]->IsNumber()){
    if(!repl_remove_edge(
          root[L"node_a_id"]->AsNumber(),
          root[L"node_b_id"]->AsNumber()))
      resp_code = 500;
    else
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
      for(uint64_t n : d.neighbors){
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
  }else{
    if(DEBUG)
      cout << "Unknown function: " << uri  << endl;
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
  
  // act as a backup no matter what
  replica_init();

  // replicate if -b flag provided
  if(master)
   master_init(ip_addr);

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
