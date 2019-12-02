#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 

int numConnections = 1;
int server_fd;
struct sockaddr_in serverAddress;

void acceptClient()
{
	struct sockaddr_in clientAddress;
	socklen_t size = sizeof(clientAddress);
	//Accept connection to client, stores address and client file descriptor
	int client_fd = accept(server_fd, (struct sockaddr*)&clientAddress, &size);

	char reply[] = "OK!";
	//Sends successful connection reply to client
	send(client_fd, "OK!", sizeof(char) * (unsigned)strlen(reply), 0);
}

int setupServer(uint16_t port, in_addr_t address)
{
	//Create Server Socket
	server_fd = socket(PF_INET, SOCK_STREAM, 0);

	//Set Address to IPv4 type
	serverAddress.sin_family = AF_INET;

	//Set Port
	serverAddress.sin_port = port;

	//Set Host Address
	serverAddress.sin_addr.s_addr = address;

	//Zero out some array?
	memset(serverAddress.sin_zero, '\0', sizeof(serverAddress.sin_zero));

	//Binds and returns return value
	return bind(server_fd, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
}

int main(int argc, char * argv[])
{
	//Sets and connects server socket to internet
	if (setupServer(htons(atoi(argv[2])), inet_addr(argv[1])) < 0) {
		printf("Bind failed\n");
		return 0;
	}
	
	//Server socket listens for up to numConnections connections
	if (listen(server_fd, numConnections) < 0) {
		printf("Listen failed\n");
		return 0;
	}
	printf("Listening\n");

	//Accepts client to receive and reply to messages
	acceptClient();	

	return 0; 
}
