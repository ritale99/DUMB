#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 

int numConnections = 5;
int server_fd;
struct sockaddr_in serverAddress;

void* handleClient(void* args)
{
	int client_fd = *(int*)args;

	char reply[] = "HELLO DUMBv0 ready!";
	//Sends successful connection reply to client
	send(client_fd, "HELLO DUMBv0 ready!", sizeof(char) * ((unsigned)strlen(reply) + 1), 0);

	//It won't be finished with a pthread_join, so should I just do yield or exit? (asking online)
	pthread_yield();
	pthread_exit(0);
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
	if (listen(server_fd, 1) < 0) {
		printf("Initial Listen failed\n");
		return 0;
	}

	//Accepts clients to receive and reply to messages
	//After each accept, create thread to handle client, then listen for another client
	while (1) {
		struct sockaddr_in clientAddress;
		socklen_t size = sizeof(clientAddress);

		//Accept a client
		int client_fd = accept(server_fd, (struct sockaddr*)&clientAddress, &size);

		//Creates a thread for connection to be handled separately
		pthread_t thread;
		pthread_create(&thread, NULL, handleClient, (void*)&client_fd);
	};


	//Unreachable, program is closed using ctrl+C
	return 0; 
}
