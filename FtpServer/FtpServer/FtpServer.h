#include "Ftp.h"

#define LISTEN_ON_PORT 1234
#define MAX_CLIENTS 10
// Status messages
#define CLIENT_DISC 0
#define CLIENT_CONN 1
#define CLIENT_X 2


struct Client {
	bool conn;			//Set true if a client is connected
	sockaddr_in addr;	//Client info like ip address
	SOCKET cs;			//Client socket
	fd_set set;			//used to check if there is data in the socket
	int i;				//any piece of additional info
	Frame frame;
	bool runner;
};


class FtpServer : public Ftp {
	private:
		struct sockaddr_in ServerAddr;		/* Server address */
		char servername[HOSTNAME_LENGTH];
		int sock;

	public:
		FtpServer();
		~FtpServer();
		
		// for main loop
		void accept_clients();
		
		void accept_new_clients();
		bool accept_client(Client *);			//accept client connections

		void getFrame(Client*);
		bool handleFrame(Client*);

		bool doUpload(Client*);
		bool doDownload(Client*);
		
		bool check_server_status();
		void updateStatus(int);
		bool disconnectClient(Client *cli);
};