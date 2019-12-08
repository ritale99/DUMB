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

void HELLO(int client_fd, char* buffer, int* sessionPtr)
{
	printf("Handling HELLO for %d\n", client_fd);

	//Handles HELLO command which creates a new session
	//session should be 1 if session setup already, 0 if not setup

	//Don't forget to check if command is HELLO, rn, we've only checked if HELLO is there, it may be HELLOQ

	char reply[] = "HELLO DUMBv0 ready!";
	send(client_fd, reply, sizeof(char) * ((unsigned)strlen(reply)+1), 0);
	return;
}

void GDBYE(int client_fd, char* buffer, int* sessionPtr)
{
	
	return;
}

void OPNBX(int client_fd, char* buffer, int* sessionPtr)
{
	printf("Good, attempting to open a box");
	return;
}


int getCommand(int client_fd, char* buffer)
{
	ssize_t readBytes = 0;
	ssize_t bytes = 6;

	printf("\tReading command of %d\n", client_fd);

	//Reads message until no more bytes found or 5 bytes read
	//5 bytes is the command length
	memset(buffer, '\0', 64);
	do {
		readBytes = read(client_fd, buffer + (6 - bytes), bytes);
		bytes -= readBytes;
		printf("\tRead %d bytes\n", readBytes);
	} while (bytes > 0 && readBytes != 0);

	if (bytes > 0) {
		return 1; //command is too short
	}
	if (readBytes < 0) {
		return 2; //error
	}
	return 0;
}


void* handleClient(void* args)
{
	//Detaches threat to make it deallocate automatically on exit
	pthread_detach(pthread_self());

	int client_fd = *(int*)args; 
	printf("Handling %d\n", client_fd);
	int session = 0;

	//Initialize buffer
	char buffer[64];
	memset(buffer, '\0', sizeof(char)*64);

	while(1) {
		//Reads message command into buffer
		int e = getCommand(client_fd, buffer);
		printf("\t%d says %s\n", client_fd, buffer);

		if (e == 1) {
			printf("Command too small\n");
			//read all bytes, but not enough bytes for a command
			//reply an error
		} else if (e == 2) {
			printf("Error reading command\n");
			break;
		} else {
			//Check what command is inputted
			if (strcmp(buffer, "HELLO") == 0) {
				HELLO(client_fd, buffer, &session);
			} else if (strcmp(buffer, "GDBYE") == 0) {
				GDBYE(client_fd, buffer, &session);
				if (session == 0) break;
			} else if (strcmp(buffer, "OPNBX") == 0) {
				OPNBX(client_fd, buffer, &session);
			}
		}
	}
	
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
		int* client_fd = (int*)malloc(sizeof(int));
		*client_fd = accept(server_fd, (struct sockaddr*)&clientAddress, &size);
		printf("Accepting %d\n", *client_fd);

		//Creates a thread for connection to be handled separately
		pthread_t thread;
		pthread_create(&thread, NULL, handleClient, (void*)client_fd);
	} 

	//Unreachable, program is closed using ctrl+C
	return 0; 
}
