/* Much of this code is adapted from the mongoose examples page,
 * https://docs.cesanta.com/mongoose/dev/#/usage-example/ */
#include <iostream>
#include <string>
#include "mongoose.h"
#include "memorygraph.hpp" 
using namespace std;

void prog_abort(string msg){
	cout << msg << endl;
	exit(1);
}

void check_args(int argc){
	if (argc < 2)
		prog_abort("Usage: ./cs426_graph_server <port>");
}

// event handler
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;
	struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
		case MG_EV_HTTP_REQUEST:
			cout << "Received HTTP request! Body:" << endl;
			cout << hm->body.p << endl;
			break;
    case MG_EV_RECV:
      // This event handler implements simple TCP echo server
			// mg_send(nc, io->buf, io->len);  // Echo received data back
			// cout << io->buf << endl;
      // mbuf_remove(io, io->len);      // Discard data from recv buffer
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
