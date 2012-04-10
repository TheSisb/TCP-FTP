#include "FtpServer.h"

// Create client Obj array
Client client[MAX_CLIENTS];
bool status = true;
int connections = 0;
int dbug;

FtpServer::FtpServer(){
	int debug;

	// Load basic starting routine  (WSAstartup & local hostname)
	Ftp::Init();

	/* Zero out structure */
	memset(&ServerAddr, 0, sizeof(ServerAddr));      
	
	ServerAddr.sin_family		=	AF_INET;					 /* Internet address family */
	ServerAddr.sin_addr.s_addr	=	htonl(INADDR_ANY);			/* Any incoming interface */
	ServerAddr.sin_port			=	htons(LISTEN_ON_PORT);         /* Local port */
	
	// Create the socket
	sock = Ftp::createSocket();

	//Bind the server socket
	debug = bind(sock,(struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) ;

	cout << "\nBinding socket:" << debug << endl;
	if(debug != 0)
		Ftp::err_sys("BIND FAILED",WSAGetLastError());
	else
		cout<<"Socket Bound to port : "<< LISTEN_ON_PORT << endl;
	
	//Successfull bind, now listen for Server requests.
	debug = listen(sock,5);
	if (debug < 0)
		Ftp::err_sys("Listen error :(");
	else
		cout<< "Server Listen mode : " << debug << endl;

	// asynchronous server tweak
	//unsigned long b=1;
	//ioctlsocket(sock,FIONBIO,&b);

	cout << "Waiting for clients!\n";
}

FtpServer::~FtpServer(){
	WSACleanup();
}

void FtpServer::accept_new_clients(){
	for(int i=0; i < MAX_CLIENTS; i++)	{
		// if open slot
		if( !(client[i].conn)) {
			// if accept client works
			if(accept_client(&client[i])) {
				client[i].runner = true;
				updateStatus(CLIENT_CONN);

				while(client[i].runner){
					// Get this clients request frame
					getFrame(&client[i]);
					//Handle request frame
					client[i].runner = handleFrame(&client[i]);
				}
			}
		}
	}
}
bool FtpServer::check_server_status(){
	return status;
}

bool FtpServer::doUpload(Client* cli){
	cout << "Sending client ready state for upload\n";
	char rdyState[2] = "u";

	dbug = send(cli->cs,rdyState,sizeof(rdyState),0);
	cout << dbug;

	recvFileAndWrite(cli->frame.filename, cli->cs);

	// check if we should close this client
	recv(cli->cs,rdyState,sizeof(rdyState),0);
	if (rdyState[0] == 'y')
		return 1;
	
	updateStatus(CLIENT_DISC);
	return 0;
}

bool FtpServer::doDownload(Client* cli){
	cout << "Sending client ready state for download\n";
	char rdyState[2] = "d";

	dbug = send(cli->cs,rdyState,sizeof(rdyState),0);
	cout << dbug;

	readFileAndSend(cli->frame.filename, cli->cs);
	
	// check if we should close this client
	recv(cli->cs,rdyState,sizeof(rdyState),0);
	if (rdyState[0] == 'y')
		return 1;
	
	updateStatus(CLIENT_DISC);
	return 0;
}

bool FtpServer::handleFrame(Client* cli){
	if (cli->frame.dir == 0) 
		return doUpload(cli);
	else if (cli->frame.dir == 1)
		return doDownload(cli);
}

void FtpServer::getFrame(Client* cli){
	Frame f;

	dbug = recv(cli->cs,(char*)&f,sizeof(Frame),0);

	cout << endl << dbug;
	cout << endl << f.dir;
	cout << endl << f.filename;

	cli->frame.dir = f.dir;
	strcpy(cli->frame.filename, f.filename);
}


bool FtpServer::accept_client(Client *cli){
	cli->i = sizeof(sockaddr); // save address
	
	cli->cs = accept(sock, (struct sockaddr *)&cli->addr, &cli->i);

	// create client thread here
	//CreateThread(NULL, 0,receive_cmds, (LPVOID)cli->cs, 0, &thread); 

	if(cli->cs != 0 && cli->cs != SOCKET_ERROR) {
		cli->conn = true;
		FD_ZERO(&cli->set);
		FD_SET(cli->cs,&cli->set);
		return true;
	}
	return false;
}

void FtpServer::updateStatus(int msg) {
	if(msg == CLIENT_CONN) {
		connections++;
		cout<<"\nWow, a client has connected. There are now " << connections << " clients connected.";
	}
	else if(msg == CLIENT_DISC) {
		connections--;
		cout<<"Wee, a client has disconnected. There are now " << connections << " clients connected.";
	} else {
		//never leave out anything
		cout<<"\n>>>>>>We got an unknown message :" << msg;
	}
}
bool FtpServer::disconnectClient(Client *cli){
	if(cli->cs)
		closesocket(cli->cs);
	cli->conn = false;
	cli->i = -1;
	updateStatus(CLIENT_DISC);
	return true;
}