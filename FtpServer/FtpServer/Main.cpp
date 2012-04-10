#include "FtpServer.h"

int main(int argc, char argv[]){
	int running = true;

	// Start the server and listen
	FtpServer* server = new FtpServer();

	while (running){
		server->accept_new_clients();
		running = server->check_server_status();

	}
	return 0;
}