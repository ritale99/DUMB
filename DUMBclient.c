#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <math.h>

//Client code
int client_fd;
struct sockaddr_in clientAddress;

int attemptConnect(char* buffer)
{
	//Attempt connect 3x
	int i;
	for (i=0; i<3; ++i) {
		printf("Connect Attempt %d\n", i);
		if (connect(client_fd, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) == 0) {
			//On connect, send HELLO
			printf("Connected\n");
			send(client_fd, "HELLO", 6, 0);
			memset(buffer, '\0', 1024);

			int bytes = 20;
			int c = 0;
			do {
				c = recv(client_fd, buffer + (20 - bytes), bytes, 0);
				bytes -= c;
				printf("Read %d bytes\n", c);
			} while (bytes > 0 && c > 0);

			//Check if server reply is correct
			if (strcmp(buffer, "HELLO DUMBv0 ready!") == 0) {
				//change this print statement
				printf("HELLO accepted\n");
				return 0;
			} else {
				printf("HELLO failed\n");
				return 1;
			}
		} else {
			printf("\t%d\n", errno);
		}
	}
	printf("Connection failed\n");
	return 1;
}

void inputString(size_t* size, char** str)
{
	char c;
	*size = 4;
	size_t len = 0;
	*str = realloc(NULL, *size); //malloc space
	if (!(*str)) {
		return;
	}

	//Source: stackoverflow.com/questions/16870485
	while (EOF != (c=fgetc(stdin)) && c != '\n') { //keep getting chars while there is input
		(*str)[len++] = c; //add char to string
		
		if (len == *size) { //if max size, increase size by 4
			*str = realloc(*str, *size += 4); //reallocate the new space
			if (!(*str)) {
				return;
			}
		}
	}

	(*str)[len++] = '\0'; //don't need to add space because there's atleast 1 space left
	*str = realloc(*str, len); //malloc just enough space
	*size = len; //set size to size of space
}

void handleInput(char** input, size_t* size)
{
	printf("Handling input\n");
	
	if (strcmp(*input, "quit") == 0) { //s
		*size = 6;
		*input = realloc(*input, *size);
		sprintf(*input, "%s", "GDBYE");
	} else if (strcmp(*input, "create") == 0) { 
		char *boxName;
		size_t nameSize;
		printf("Please input the name of the message box to create between 5 and 25 characters:\n");
		inputString(&nameSize, &boxName);
		*size = 5 + 1 + floor(log10((int)nameSize)) + 1 + nameSize + 1;
 		*input = realloc(*input, *size);
		sprintf(*input, "%s!%d!%s", "CREAT", nameSize, boxName);
		free(boxName);	
	} else if (strcmp(*input, "delete") == 0) { //r
		char *boxName;
		size_t nameSize;
		printf("Please input the name of the message box to delete:\n");
		inputString(&nameSize, &boxName);
		*size = 5 + 1 + floor(log10((int)nameSize)) + 1 + nameSize;
 		*input = realloc(*input, *size);
		sprintf(*input, "%s!%d!%s", "DELBX", nameSize, boxName);
		free(boxName);
	} else if (strcmp(*input, "open") == 0) { //s
		char* boxName;
		size_t nameSize;
		printf("Please input the name of the message box to open:\n");
		//takes user input
		inputString(&nameSize, &boxName);

		//sum of cmd, !, length of size, !, message
		*size = 5 + 1 + floor(log10((int)nameSize)) + 1 + nameSize + 1;
		//reallocate input to fit new input
		*input = realloc(*input, *size);
		//set new input
		sprintf(*input, "%s!%d!%s", "OPNBX", nameSize, boxName);

		//free inputted name of box
		free(boxName);
	} else if (strcmp(*input, "close") == 0) { //s
		char *boxName;
		size_t nameSize;
		printf("Please input the name of the message box to delete:\n");
		inputString(&nameSize, &boxName);
		*size = 5 + 1 + floor(log10((int)nameSize)) + 1 + nameSize;
 		*input = realloc(*input, *size);
		sprintf(*input, "%s!%d!%s", "CLSBX", nameSize, boxName);
		free(boxName);
	} else if (strcmp(*input, "next") == 0) { //r
		*size = 6;
		*input = realloc(*input, *size);
		sprintf(*input, "%s", "NXTMG");


	} else if (strcmp(*input, "put") == 0) { //s
		char *boxName;
		size_t nameSize;
		printf("Please input your message\n");
		inputString(&nameSize, &boxName);
		*size = 5 + 1 + floor(log10((int)nameSize)) + 1 + nameSize+1;
 		*input = realloc(*input, *size);
		sprintf(*input, "%s!%d!%s", "PUTMG", nameSize, boxName);
		free(boxName);	
	} else if (strcmp(*input, "help") == 0){
		printf("Commands are:\n\tcreate\n\tdelete\n\topen\n\tclose\n\tnext\n\tinput\n");
	}
		
	return;
}

int handleReply(char* input, char* reply)
{
	//get reply
	memset(reply, '\0', 1024);
	
	printf("Handling reply\n");
	int bytes = 3;
	int readBytes = 0;
	do {
		readBytes = recv(client_fd, reply + (3 - bytes), bytes, 0);
		bytes -= readBytes;
		printf("\tRead %d bytes\n", readBytes);
	} while (bytes > 0 && readBytes > 0);

	//Format input to ignore arguments
	input[5] = '\0';

	//On successful command
	printf("\tServer replied %s\n", reply);
	if (strcmp(reply, "OK!") == 0) {
		if (strcmp(input, "CREAT") == 0) {
			//On successful CREAT
		} else if (strcmp(input, "OPNBX") == 0) {
			//ON successful OPNBX
		} else if (strcmp(input, "NXTMG") == 0) {

		} else if (strcmp(input, "PUTMG") == 0) {

		} else if (strcmp(input, "DELBX") == 0) {
			
		}
	//On Error
	} else if (strcmp(reply, "ER:") == 0) {
		//Get error message
		int bytes2 = 6;
		do {
			readBytes = recv(client_fd, reply + 3 + (6 - bytes2), bytes2, 0);
			bytes2 -= readBytes;
			printf("\tRead %d bytes\n", readBytes);
		} while (bytes2 > 0 && readBytes > 0);

		printf("\t\t%s\n", reply);

		if (strcmp(reply, "ER:EXIST")==0) {
			//Attempt to create inbox with a used name
			printf("Attempting to create inbox with a used name\n");
		} else if (strcmp(reply, "ER:WHAT?")==0) {
			//Broken or malformed input
			printf("Malformed Input\n");
		} else if (strcmp(reply, "ER:NEXST")==0) {
			//Attempt to open/delete not existing inbox
		} else if (strcmp(reply, "ER:OPEND")==0) {
			//Attempt to open/delete already opened inbox
		} else if (strcmp(reply, "ER:EMPTY")==0) {
			//Attempt to read empty inbox
		} else if (strcmp(reply, "ER:NOOPN")==0) {
			//Attempt to read/write/close with no inbox open
		} else if (strcmp(reply, "ER:NOTMY")==0) { 
			//Attempt to delete not empty inbox
		} 
	} else if (bytes == 3 && readBytes == 0) {
		//Connection closed
		printf("Connection closed.\n");
		exit(0);
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
	printf("Setup Client\n");

	return;
}

int main(int argc, char* argv[])
{
	char reply[1024];

	setupClient(htons(atoi(argv[2])), inet_addr(argv[1]));

	//Attempt to connect to server
	if (attemptConnect(reply) == 1) return 0;

	//continue entering messages
	while(1) {
		printf("\nEnter a command:\n");
		char* input;
		size_t size;
		inputString(&size, &input);

		//change input to a command and ask for arguments if necessary
		handleInput(&input, &size);

		//send input to server
		printf("\tSending %s with size %d\n", input, size);
		send(client_fd, input, size, 0);

		//receives and handles reply from server
		if (handleReply(input, reply) == 1) {
			free(input);
			break;
		}
		free(input);
	}

	return 0; 
}
