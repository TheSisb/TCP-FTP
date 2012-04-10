#include "Ftp.h"

void Ftp::Init(){
	cout << "/***********************************/\n";
	cout << "/*  Welcome to ShadiFTP (server)   */\n";
	cout << "/***********************************/\n\n";

	// Initialize WinSock
	if (WSAStartup(0x0202,&wsaData) != 0)	{  
		WSACleanup();  
	    err_sys("Error in starting WSAStartup()\n");
	}
	cout << "Winsock started.\n";
	// Get and print our own hostname
	if(gethostname(myHostname, HOSTNAME_LENGTH) != 0 ) 
		err_sys("can not get the host name,program exit");
	cout << "Starting at host: " << myHostname << endl;
}

int Ftp::createSocket(){
	//Create the socket
	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(sock==INVALID_SOCKET )
		err_sys("Invalid Socket ",WSAGetLastError());
	else if(sock==SOCKET_ERROR)
		err_sys("Socket Error)",WSAGetLastError());
	else
		cout<<"\nSOCKET Established"<<endl;

	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)(1),sizeof (1));
	
	return sock;
}

unsigned long Ftp::ResolveName(char name[]) {
	struct hostent *host;            /* Structure containing host information */
	
	if ((host = gethostbyname(name)) == NULL)
		err_sys("gethostbyname() failed");
	
	/* Return the binary, network byte ordered address */
	return *((unsigned long *) host->h_addr_list[0]);
}

//from Richard Stevens's source code
void Ftp::err_sys(char * fmt,...) {     
	int x;
	perror(NULL);
	va_list args;
	va_start(args,fmt);
	fprintf(stderr,"error: ");
	vfprintf(stderr,fmt,args);
	fprintf(stderr,"\n");
	va_end(args);
	std::cin >> x;
	exit(1);
}

bool Ftp::file_exists(char * filename){
	ifstream ifile(filename);
	return ifile;
}

bool Ftp::readFileAndSend(char* filename, SOCKET sock){
	Packet p;
	p.header = 1;
	p.footer = 0;
	int test;

	// Open
	cout << "Opening the file for Reading...\n";
	ifstream is (filename, ios::in | ios::binary);

	// Check if error
	if ( !is )  {
		cout << "Error opening the file to read :/\n";
		return 0;
	}

	// Read and Write entire contents
	while ( is ) {
		// Read
		is.read(p.buffer, BUFFER_SIZE);
		//cout << "\nInput file pointer at: " << is.tellg() << "\nAmount read: " << is.gcount();
		
		// eof + last packet
		if (is.gcount() < BUFFER_SIZE)
			p.footer = is.gcount(); 
		
		test = send(sock,(char*)&p,sizeof(Packet),0);
		//cout << test << endl;
		
	}

	// Close stuff
	cout << "closing read file.\n";
	is.close();
	return 1;
}



bool Ftp::recvFileAndWrite(char* filename, SOCKET sock){
	Packet p;
	p.header = 1;
	p.footer = 0;
	int test;

	char fname[FILENAME_LENGTH+1] = "2";
	strcat(fname, filename);

	// Open 
	cout << "Opening the file for writing...\n";
	ofstream os (fname, ios::out | ios::binary);

	// Check if error
	if ( !os )  {
		cout << "Error opening the file to write :/\n"; 
		return 0;
	}
	int i=0;
	while ( true ) {
		//cout << i << endl;
		test = recv(sock,(char*)&p, sizeof(Packet),0);
		//cout << test;
		if(p.footer == 0)
			os.write(p.buffer, BUFFER_SIZE);
		else{
			os.write(p.buffer, p.footer);
			//cout << "last packet sent of size: " << p.footer << endl;
			break;
		}
		i++;
	}
	cout << "closing written file.\n";
	os.close();
	return 1;
}