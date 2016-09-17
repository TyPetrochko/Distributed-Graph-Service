/* Note that this code draws from the c networking tutorial found at
 * http://www.linuxhowtos.org/C_C++/socket.htm.
 */
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "memorygraph.hpp"
#include "mongoose.h"

using namespace std;

#define BUFFER_SIZE (1024)

void process_request(int client_socket);

void abort (string msg){
	cout << msg << endl;
	exit(1);
}

void check_args(int argc){
	if (argc < 2)
		abort("Usage: ./cs426_graph_server <port>");
}

int main(int argc, char *argv[])
{
	check_args(argc);

	int sockfd; 		// welcome socket
	int newsockfd; 	// client socket
	int portno; 		// port to operate on
	int clilen; 		// length of client's address

	// our address and the client's
	struct sockaddr_in serv_addr, cli_addr;

	// ask the OS to open the welcome socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		abort("ERROR opening socket");

	// zero out address buffer
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	// configure server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = local address

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		  abort("ERROR on binding");

	// listen with a backlog of 5 requests (this is system max)
	listen(sockfd, 5);

	// accept a request (blocks here!)
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
			(socklen_t*) &clilen);
	if (newsockfd < 0)
		  abort("ERROR on accept");

	process_request(newsockfd);

	// zero the buffer, and read in up to 255 chars
	cout << "Done!" << endl;
}

// handle an incoming request from a client
void process_request(int client_socket){
	char buffer[BUFFER_SIZE]; // a buffer to read chars from socket
	int n, n2;				// number of bytes read or written

	while(true){
		// clear buffer and read in from socket
		bzero(buffer, BUFFER_SIZE);
		n = read(client_socket, buffer, BUFFER_SIZE);
		if (n < 0) 
			abort("ERROR reading from socket");

		cout << buffer;

		// echo back to client
		n2 = send(client_socket, buffer, n, MSG_NOSIGNAL);
		if (n2 == -1 || n != n2){
			close(client_socket);
			return;
		}
	}
}
