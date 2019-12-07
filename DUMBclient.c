#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h> 

int clientSocket;
int client_fd;
struct sockaddr_in clientAddress;

int attemptConnect(socklen_t addr_size, char* buffer)
{
	//Attempt connect 3x
	int i;
	for (i=0; i<3; ++i) {
		if (connect(clientSocket, (struct sockaddr*)&clientAddress, addr_size) >= 0) {
			//On connect, send HELLO
			send(clientSocket, "HELLO", 6, 0);
			memset(buffer, '\0', 1024);

			int bytes = 20;
			int c = 0;
			do {
				c = recv(clientSocket, buffer (bytes - 3), bytes, 0);
				bytes -= c;
			} while (bytes > 0 && c > 0);

			//Check if server reply is correct
			if (strcmp(buffer, "HELLO DUMBv0 ready!") == 0) {
				printf("Connection accepted\n");
				return 0;
			} else {
				printf("Connection failed: HELLO failed\n");
				return 1;
			}
		}
	}
	printf("Connection failed\n");
	return 1;
}

void handleInput(char* input)
{
	if (strcmp(input, "quit")) {
		
	} else if (strcmp(input, "create")) {
		//Convert "create" to "CREAT"
		input = "CREAT "; //Note, makes input[5] a '\0' already, so don't need to manually do it
		printf("Please input the name of the new message box:\n");
		scanf("%s", input + 6);
	} else if (strcmp(input, "delete")) {

	} else if (strcmp(input, "open")) {

	} else if (strcmp(input, "close")) {

	} else if (strcmp(input, "next")) {

	} else if (strcmp(input, "put")) {

	}
}

int handleReply(char* input, char* reply)
{
	//get reply
	memset(reply, '\0', 1024);
	
	int bytes = 3;
	int c = 0;
	do {
		c = recv(clientSocket, reply + (bytes - 3), bytes, 0);
		bytes -= c;
	} while (bytes > 0 && c > 0);

	//Format input to ignore arguments
	input[5] = '\0';

	//On successful command
	if (strcmp(reply, "OK!")) {
		printf("Server replied %s\n", reply);

		if (strcmp(input, "GDBYE") == 0) {
			//On successful GDBYE? no reply, I think
		} else if (strcmp(input, "CREAT") == 0) {
			//On successful CREAT
		} else if (strcmp(input, "OPNBX") == 0) {
			//ON successful OPNBX
		} else if (strcmp(input, "NXTMG") == 0) {

		} else if (strcmp(input, "PUTMG") == 0) {

		} else if (strcmp(input, "DELBX") == 0) {
			
		}
	//On Error
	} else if (strcmp(reply, "ER:")) {
		//Get error message
		int bytes2 = 5;
		do {
			c = recv(clientSocket, reply + 3 + (bytes2 - 5), bytes2, 0);
			bytes2 -= c;
		} while (bytes2 > 0 && c > 0);

		printf("Server replied %s\n", reply);

		if (strcmp(reply, "ER:EXIST")) {
			//Attempt to create inbox with a used name
		} else if (strcmp(reply, "ER:WHAT?")) {
			//Broken or malformed input
		} else if (strcmp(reply, "ER:NEXST")) {
			//Attempt to open/delete not existing inbox
		} else if (strcmp(reply, "ER:OPEND")) {
			//Attempt to open/delete already opened inbox
		} else if (strcmp(reply, "ER:EMPTY")) {
			//Attempt to read empty inbox
		} else if (strcmp(reply, "ER:NOOPN")) {
			//Attempt to read/write/close with no inbox open
		} else if (strcmp(reply, "ER:NOTMY")) {
			//Attempt to delete not empty inbox
		}
	}
	return 0;
}

void setupClient(uint16_t port, in_addr_t address)
{
	//Create client Socket
	client_fd = socket(PF_INET, SOCK_STREAM, 0);

	//Set Address to IPv4 type
	clientAddress.sin_family = AF_INET;

	//Set Port
	clientAddress.sin_port = port;

	//Set Host Address
	clientAddress.sin_addr.s_addr = address;

	//Buffer of zeros
	memset(clientAddress.sin_zero, '\0', sizeof(clientAddress.sin_zero));

	return;
}

int main(int argc, char* argv[])
{
	char input[1024];
	char reply[1024];

	setupClient(htons(atoi(argv[2])), inet_addr(argv[1]));
	socklen_t addr_size = sizeof(clientAddress.sin_addr.s_addr);

	//Attempt to connect to server
	if (attemptConnect((struct sockaddr*)&address, addr_size, reply) == 1) return 0;

	//continue entering messages
	while(1) {
		printf("Enter the Message\n");
		memset(input, '\0', 1024);
		//takes a command as input
		scanf("%s", input);

		//change input to a command and ask for arguments if necessary
		handleInput(input);

		//send input to server
		send(clientSocket, input, strlen(input), 0);

		//receives and handles reply from server
		if (handleReply(input, reply) == 1) break;
	}

	return 0; 
}
