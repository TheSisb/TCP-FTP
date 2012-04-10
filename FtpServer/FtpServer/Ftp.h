#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

#define HOSTNAME_LENGTH 30
#define FILENAME_LENGTH 30
#define BUFFER_SIZE 512
#define LISTEN_PORT 1234

typedef struct {
	int header;
	char buffer[BUFFER_SIZE];
	int footer;
} Packet;

typedef struct {
	int dir;
	char filename[FILENAME_LENGTH];
} Frame;


class Ftp {
	private:
		int sock;						/* Socket descriptor */
		struct sockaddr_in ServAddr;	/* server socket address */
		unsigned short ServPort;		/* server port */	
		WSADATA wsaData;
		char myHostname[HOSTNAME_LENGTH];


	public:
		void Init();
		int createSocket();
		unsigned long ResolveName(char name[]);
		void err_sys(char * fmt,...);

		bool file_exists(char * filename);
		bool readFileAndSend(char* name, SOCKET sock);
		bool recvFileAndWrite(char * filename, SOCKET sock);
		
};
