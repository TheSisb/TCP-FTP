#include "FtpClient.h"

FtpClient::FtpClient(){
	// Load basic starting routine  (WSAstartup & local hostname)
	Ftp::Init();

	// Create the socket
	sock = Ftp::createSocket();
	
	cout << "Please enter the hostname to connect to: ";
	cin >> remoteHostname;

	//connect to the server
	memset(&ServerAddr, 0, sizeof(ServerAddr));     /* Zero out structure */

	ServerAddr.sin_family      = AF_INET;             /* Internet address family */
	ServerAddr.sin_addr.s_addr = ResolveName(remoteHostname);   /* Server IP address */
	ServerAddr.sin_port        = htons(CONNECT_ON_PORT); /* Server port */

	if (connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		err_sys("Socket Creating Error");
	cout << "\nConnected to the server successfully\n";
}

FtpClient::~FtpClient(){
	WSACleanup();
}

bool FtpClient::run(){
	Frame fr;
	bool running = true;
	while(running){
		cout << "Would you like to UPLOAD (0) or DOWNLOAD (1) or EXIT (else)?\n";
		cin >> fr.dir;
		
		if (fr.dir != 0 && fr.dir != 1)
			doShutdown();
		
		cout << "\nWhich file?\n";
		cin >> fr.filename;


		cout << "Notifying server of action...\n";
		debug = send(sock,(char*)&fr,sizeof(Frame),0);
		cout << "Debug = " << debug << endl;
	
		cout << "Receiving reply: ";
		char rdyState[2];
		debug = recv(sock,rdyState,sizeof(rdyState),0);
		cout << debug << " " << rdyState << endl;

		switch (fr.dir) {
			case 0:
				cout <<"so.. we're uploading \n";
				Ftp::readFileAndSend(fr.filename, sock);
				break;
			case 1:
				cout <<"so.. we're downloading \n";
				Ftp::recvFileAndWrite(fr.filename, sock);
				break;
			default:
				doShutdown();
				break;
		}
		cout << "\nWould you like to go again? y/n: ";
		cin >> rdyState;
		cout << endl << rdyState << endl;
		if (rdyState[0] == 'n')
			running = false;
		send(sock,rdyState,sizeof(rdyState),0);
	}
	cin >> debug;
	return 1;
}

bool FtpClient::doUpload(Frame f){
	if (!file_exists(f.filename))
		//send size 0 to notify of shutdown
		err_sys("That file doesn't exist!\n");
	return 1;
}

bool FtpClient::doDownload(Frame f){


	return 1;
}



bool FtpClient::doShutdown(){
	cout << "\nClosing! <3";

	// shutdown the send half of the connection since no more data will be sent
	if (shutdown(sock, 2) == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(sock);
	WSACleanup();
	return 0;
}