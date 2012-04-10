#include "Ftp.h"

#define CONNECT_ON_PORT 1234

class FtpClient : public Ftp {

	private:
		struct sockaddr_in ServerAddr;		/* Server address */
		char remoteHostname[HOSTNAME_LENGTH];
		int sock;
		int debug;

	public:
		FtpClient();
		~FtpClient();
		
		bool run();
		
		bool doDownload(Frame);
		bool doUpload(Frame);
		bool doShutdown();

		

};