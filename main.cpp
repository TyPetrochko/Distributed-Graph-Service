/* Much of this code is adapted from the mongoose examples page,
 * https://docs.cesanta.com/mongoose/dev/#/usage-example/ */

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "JSON.h"
#include "mongoose.h"

#include "persistence.hpp"
#include "memorygraph.hpp"
#include "util.hpp"

using namespace std;

// default error message
string err_msg = string("HTTP/1.1 400 Bad Request\r\n")
  + "Content-Length: 0\r\n"
  +	"Content-Type: application/json\r\n";

// flags to read in
char* port;
bool format;
string dev_file;

void process_args(int argc, char **argv) {
	if (argc < 3){
		cerr << "Usage: ./cs426_graph_server [-f] <port> <devfile>" << endl;
		exit(1);
	}

	if (argc == 3) {
	   	format = false;
	    port = argv[1];
	    dev_file = string(argv[2]);
	} else if (argc == 4) {
	    format = true;
	    port = argv[2];
	    dev_file = string(argv[3]);
	}
}

// convert mongoose's weird string struct to a regular c++ string
string mg_str_to_string(struct mg_str m) {
	string s = string(m.p).substr(0, m.len);
	return s;
}

// convert a json object to a c++ string (supports unicode)
string json_to_string(JSONObject j) {
	wstring ws((new JSONValue(j))->Stringify());
	string str(ws.begin(), ws.end());
	return str;
}

// handle an incoming api call
string handle_request(string body, string uri) {
	JSONValue *value;
	JSONObject root;
	string func;
	string prefix = "/api/v1/";

	// parse any json in the request
	value = JSON::Parse(body.c_str());
	if (value == NULL || value->IsObject() == false) {
		return err_msg;
	}
	root = value->AsObject();

	// remove prefix from uri
	func = uri;
	if (func.length() <= prefix.length()) {
		return err_msg;
	}
	func.erase(0, prefix.length());

	// initialize default values for response
	string payload  = "";
	string proto = "HTTP/1.1";
	int resp_code = 400;

	// giant pseudo-switch statement to actually call the API
	if (func == "add_node" && root[L"node_id"] && root[L"node_id"]->IsNumber()) {
		resp_code = add_node(root[L"node_id"]->AsNumber());
	    if (!log_add_node(root[L"node_id"]->AsNumber()))
	      resp_code = 507;
	    else
	      // resp_code = add_node(root[L"node_id"]->AsNumber());
			if(resp_code == 200) payload = body;
	} else if (func == "add_edge" 
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
    if (!log_add_edge(
        root[L"node_a_id"]->AsNumber(),
        root[L"node_b_id"]->AsNumber()))
      resp_code = 507;
    else
      resp_code = add_edge(
          root[L"node_a_id"]->AsNumber(),
          root[L"node_b_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	} else if (func == "remove_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
    if (!log_remove_node(root[L"node_id"]->AsNumber()))
      resp_code = 507;
    else
      resp_code = remove_node(root[L"node_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	} else if (func == "remove_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()) {
    if (!log_remove_edge(
        root[L"node_a_id"]->AsNumber(),
        root[L"node_b_id"]->AsNumber()))
      resp_code = 507;
    else
      resp_code = remove_edge(
          root[L"node_a_id"]->AsNumber(),
          root[L"node_b_id"]->AsNumber());
		if(resp_code == 200) payload = body;
	} else if (func == "get_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()) {
		struct nodeData d = get_node(root[L"node_id"]->AsNumber());
		resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		payload = json_to_string(return_data).c_str();
	} else if (func == "get_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()) {
		struct nodeData d = get_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		payload = json_to_string(return_data).c_str();
	} else if (func == "get_neighbors" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()) {
		struct neighborData d = get_neighbors(root[L"node_id"]->AsNumber());
		if ((resp_code = d.status) == 200) {
			JSONObject return_data;
			JSONArray neighbors;
			for(uint64_t n : d.neighbors) {
				neighbors.push_back(new JSONValue((double) n));
			}
			return_data[L"node_id"] = root[L"node_id"];
			return_data[L"neighbors"] = new JSONValue(neighbors);
			payload = json_to_string(return_data);
		}
	} else if (func == "shortest_path"
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()) {
		struct distanceData d = shortest_path(
				root[L"node_a_id"]->AsNumber(), 
				root[L"node_b_id"]->AsNumber());
		if ((resp_code = d.status) == 200) {
			JSONObject return_data;
			return_data[L"distance"] = new JSONValue((int) d.distance);
			payload = json_to_string(return_data).c_str();
		}
	} else if (func == "checkpoint") {
		bool checkpoint_success = checkpoint();
		if (checkpoint_success) {
			resp_code = 200;
		} else {
			resp_code = 507;
		}
	} else {
		return err_msg;
	}

	// give appropriate status message
	string resp_status_msg;
	if (resp_code == 400 || resp_code == 507)
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

				// close the connection!
				nc->flags |= MG_F_SEND_AND_CLOSE;
				mbuf_remove(io, io->len);
			}
			break;
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
	process_args(argc, argv);

	init(dev_file, format);

	struct mg_connection *nc;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);  // Initialize event manager object

	// Note that many connections can be added to a single event manager
	// Connections can be created at any point, e.g. in event handler function
	nc = mg_bind(&mgr, port, ev_handler);

	// listen for http
	mg_set_protocol_http_websocket(nc);

	if(VERBOSE) {
	  	print_nodes();
	    print_graph();
	    cout << "nodes and graph later in main\n";
	}

	for (;;) {  // Start infinite event loop
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	return 0;
}
