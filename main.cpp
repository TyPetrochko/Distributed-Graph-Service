/* Much of this code is adapted from the mongoose examples page,
 * https://docs.cesanta.com/mongoose/dev/#/usage-example/ */
#include <iostream>
#include <vector>
#include <string>
#include "mongoose.h"
#include "memorygraph.hpp"
#include "JSON.h"
#include <stdlib.h>
using namespace std;

void prog_abort(string msg){
	cout << msg << endl;
	exit(1);
}

void prog_assert(int condition, string err_msg){
	if(!condition)
		prog_abort(err_msg);
}

void check_args(int argc){
	if (argc < 2)
		prog_abort("Usage: ./cs426_graph_server <port>");
}

string mg_str_to_string(struct mg_str m){
	string s = string(m.p).substr(0, m.len);
	return s;
}

string json_to_string(JSONObject j){
	wstring ws((new JSONValue(j))->Stringify());
	string str(ws.begin(), ws.end());
	return str;
}

struct http_message handle_request(string body, string uri){
	JSONValue *value;
	JSONObject root;
	string func;
	string prefix = "/api/v1/";

	// parse any json in the request
	value = JSON::Parse(body.c_str());
	if(value == NULL || value->IsObject() == false){
		prog_abort("Couldn't parse JSON: " + body);
	}
	root = value->AsObject();

	// remove prefix from uri
	func = uri;
	if(func.length() <= prefix.length()){
		prog_abort("Bad URI: " + func);
	}
	func.erase(0, prefix.length());

	// make a blank request
	struct http_message response = {};
	response.proto = mg_mk_str("HTTP/1.1");
	response.body = mg_mk_str("");

	cout << "FUNCTION=<" << func << ">" << endl;
	// giant pseudo-switch statement to actually call the API
	if(func == "add_node" && root[L"node_id"] && root[L"node_id"]->IsNumber()){
		response.resp_code = add_node(root[L"node_id"]->AsNumber());
		if(response.resp_code == 200) response.body = mg_mk_str(body.c_str());
	}else if (func == "add_edge" 
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		response.resp_code = add_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		if(response.resp_code == 200) response.body = mg_mk_str(body.c_str());
	}else if (func == "remove_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		response.resp_code = remove_node(root[L"node_id"]->AsNumber());
		if(response.resp_code == 200) response.body = mg_mk_str(body.c_str());
	}else if (func == "remove_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		response.resp_code = remove_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		if(response.resp_code == 200) response.body = mg_mk_str(body.c_str());
	}else if (func == "get_node" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		struct nodeData d = get_node(root[L"node_id"]->AsNumber());
		response.resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		response.body = mg_mk_str(json_to_string(return_data).c_str());
	}else if (func == "get_edge"
			&& root[L"node_a_id"]
			&& root[L"node_b_id"]
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		struct nodeData d = get_edge(
				root[L"node_a_id"]->AsNumber(),
				root[L"node_b_id"]->AsNumber());
		response.resp_code = d.status;
		JSONObject return_data;
		return_data[L"in_graph"] = new JSONValue(d.in_graph);
		response.body = mg_mk_str(json_to_string(return_data).c_str());
	}else if (func == "get_neighbors" 
			&& root[L"node_id"] 
			&& root[L"node_id"]->IsNumber()){
		struct neighborData d = get_neighbors(root[L"node_id"]->AsNumber());
		if((response.resp_code = d.status) == 200){
			JSONObject return_data;
			JSONArray neighbors;
			for(uint64_t n : d.neighbors){
				neighbors.push_back(new JSONValue((double) n));
			}
			return_data[L"neighbors"] = new JSONValue(neighbors);
			response.body = mg_mk_str(json_to_string(return_data).c_str());
		}
	}else if (func == "shortest_path"
			&& root[L"node_a_id"]->IsNumber() 
			&& root[L"node_b_id"]->IsNumber()){
		struct distanceData d = shortest_path(
				root[L"node_a_id"]->AsNumber(), 
				root[L"node_b_id"]->AsNumber());
		if((response.resp_code = d.status) == 200){
			JSONObject return_data;
			return_data[L"distance"] = new JSONValue((int) d.distance);
			response.body = mg_mk_str(json_to_string(return_data).c_str());
		}
	}

	// give appropriate status message
	if(response.resp_code == 400)
		response.resp_status_msg = mg_mk_str("Bad Request");
	else
		response.resp_status_msg = mg_mk_str("OK");

	/* Optional for Debugging
	vector<wstring> keys = value->ObjectKeys();
	wstring s;
	for (vector<wstring>::iterator it = keys.begin(); it != keys.end(); ++it){
		s = *it;
		wcout << "KEY: " << s << endl;
		wcout << "VALUE: " << root[s]->AsNumber() << endl;
	}

	cout << "HEREWEGO: " << root[L"node_id"]->AsNumber() << endl;
	*/

	return response;
}

// event handler
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	struct mbuf *io = &nc->recv_mbuf;
	struct mbuf *out = &nc->send_mbuf;
	switch (ev) {
		case MG_EV_HTTP_REQUEST:
			{
				struct http_message resp = handle_request(
						mg_str_to_string(hm->body), mg_str_to_string(hm->uri));
				
				// build header
				string headers_string = "Content-Length: " 
					+ to_string(resp.body.len)
					+ "\r\n"
					+ "Content-Type: application/json";

				// build text to send
				string to_send = mg_str_to_string(resp.proto)
					+ " "
					+ to_string(resp.resp_code)
					+ " "
					+ mg_str_to_string(resp.resp_status_msg)
					+ "\r\n"
					+ headers_string;
				if(resp.body.p != NULL)
					to_send = to_send + "\r\n\r\n" + mg_str_to_string(resp.body);
				
				to_send += "\r\n";

				//send it
				mg_send(nc, to_send.c_str(), to_send.length());

				// debug!
				cout << "BEGIN RESPONSE" << endl;
				cout << to_send << endl;
				cout << "END RESPONSE" << endl;

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
	check_args(argc);

	struct mg_connection *nc;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);  // Initialize event manager object

  // Note that many connections can be added to a single event manager
  // Connections can be created at any point, e.g. in event handler function
  nc = mg_bind(&mgr, argv[1], ev_handler);
	
	// listen for http
	mg_set_protocol_http_websocket(nc);

	for (;;) {  // Start infinite event loop
    mg_mgr_poll(&mgr, 1000);
  }

  mg_mgr_free(&mgr);
  return 0;
}
