#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include <errno.h>

int numConnections = 5;
int server_fd;
struct sockaddr_in serverAddress;

void HELLO(int client_fd, char* buffer) {
	return;
}

int readMessage(int client_fd, char* buffer) {
	ssize_t readBytes = 0
	ssize_t bytes = 5;

	//Reads message until no more bytes found or 5 bytes read
	//5 bytes is the command length
	do {
		readBytes = read(client_fd, buffer, bytes);
		bytes -= readBytes;
	} while (bytes > 0 && readBytes > 0);

	if (bytes > 0) {
		return 1; //Invalid command
	}
	return 0;
}

void* handleClient(void* args)
{
	//Detaches threat to make it deallocate automatically on exit
	pthread_detach(pthread_self());

<<<<<<< HEAD
	int client_fd = *(int*)args;
	int session = 0;

	//Initialize buffer
	char buffer[64];
	memset(buffer, '\0', sizeof(char)*64);

	while(1) {
		//Reads message command into buffer
		if (getCommand(client_fd, buffer)) {
			//Invalid command
		}

		int handler = handleCommand(client_fd, buffer);
		if (handler < 0) {
			//error
		} else if (handler == 0) { //HELLO
			HELLO(client_fd, buffer);
		} else if (handler == 1) { //GDBYE
			
		} 
	}
	
	//Sends successful connection reply to client
	send(client_fd, reply, sizeof(char) * ((unsigned)strlen(reply) + 1), 0);

	
=======
	char reply[] = "HELLO DUMBv0 ready!";
	//Sends successful connection reply to client
	send(client_fd, "HELLO DUMBv0 ready!", sizeof(char) * ((unsigned)strlen(reply) + 1), 0);
>>>>>>> 3d01f7dbdc0cef5fc16b50a18b63538bad8cb8d4

	//It won't be finished with a pthread_join, so should I just do yield or exit? (asking online)
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
	
	//Listens for and Accepts clients to receive and reply to messages
	//After each accept, create thread to handle client, then listen for another client
	while (1) {
		if (listen(server_fd, 1) < 0) {
			printf("Listen failed: %d\n", errno);
			return 0;
		}

		struct sockaddr_in clientAddress;
		socklen_t size = sizeof(clientAddress);

		//Accept a client's message
		int client_fd = accept(server_fd, (struct sockaddr*)&clientAddress, &size);
		
		//Creates a thread for connection to be handled separately
		pthread_t thread;
		pthread_create(&thread, NULL, handleClient, (void*)&client_fd);
	};


	//Unreachable, program is closed using ctrl+C
	return 0; 
}
