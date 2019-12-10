#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include <errno.h>

//Server Code

int server_fd;
struct sockaddr_in serverAddress;

struct message {
	char* message; //String
	struct message* next; //Next message
};
struct inbox {
	char* name; //Name of inbox
	struct message* message1; //Pointer to message of select inbox
	pthread_mutex_t lock; //Locks the int busy
	int user; //0 if not in use, client_id if in use
	struct inbox* next; //Pointer to next inbox
};
struct inbox* inbox1 = NULL; //Head of inbox list

int getLengthFromMessage(int client_fd, char** buffer, size_t* bufferSize)
{
	memset(*buffer, '\0', *bufferSize);

	*bufferSize = 4;
	size_t len = 0;
	int readBytes = 0;
	*buffer = realloc(*buffer, *bufferSize);

	//Read until ! is found
	do {
		readBytes = read(client_fd, (*buffer) + (len++), 1);
		if (len == *bufferSize) { //Add space if at end
			*buffer = realloc(*buffer, (*bufferSize) += 4); 
		}
	} while ((*buffer)[len - 1] != '!' && readBytes > 0);

	if (readBytes <= 0) {
		return -1; //Either error(<0) or EOF(0)
	}
	
	(*buffer)[len - 1] = '\0';

	return atoi(*buffer); //Returns as int, length of message and leaves cursor after !
}

int readNBytes(int client_fd, char** buffer, size_t* bufferSize, int n)
{
	printf("Reading %d bytes\n", n);
	memset(*buffer, '\0', *bufferSize);

	*bufferSize = n;
	int readBytes = 0;
	*buffer = realloc(*buffer, *bufferSize);

	//Read n times
	do {
		readBytes = read(client_fd, (*buffer) + (*bufferSize) - n, n);
		n -= readBytes;
		printf("\tRead %d bytes\n", readBytes);
	} while (n > 0 && readBytes > 0);

	printf("\tRead %s\n", *buffer);

	if (readBytes < 0) {
		return 1; //Error, not sure why
	}
	if (n > 0) {
		return 1; //given length is longer than message 
	}

	return 0;
}

void HELLO(int client_fd, char** buffer, size_t* bufferSize)
{
	printf("Handling HELLO for %d\n", client_fd);

	//Handles HELLO command which creates a new session
	//session should be 1 if session setup already, 0 if not setup

	//Don't forget to check if command is HELLO, rn, we've only checked if HELLO is there, it may be HELLOQ

	char reply[] = "HELLO DUMBv0 ready!";
	send(client_fd, reply, (unsigned)strlen(reply) + 1, 0);
	return;
}
void GDBYE(int client_fd, char** buffer, size_t* bufferSize)
{
	printf("Handling GDBYE for %d\n", client_fd);

	//close socket
	close(client_fd);
	
	free(*buffer);
	printf("Client %d disconnected\n", client_fd);
	pthread_exit(0);
	return;
}
void OPNBX(int client_fd, char** buffer, size_t* bufferSize, struct inbox** currentInbox, struct message** currentMsg)
{
	printf("Handling OPNBX for %d\n", client_fd);

	//Read length of box name
	int messageSize = getLengthFromMessage(client_fd, buffer, bufferSize);
	printf("\tInbox name size is %d\n", messageSize);

	//Read box name from client message
	if (readNBytes(client_fd, buffer, bufferSize, messageSize) == 1) {
		printf("\tProblem readingNBytes\n");
		return;
	}
	printf("\tSearching for %s\n", *buffer);

	//Check each inbox for search box from start to end
	struct inbox* targetInbox = inbox1;
	while (targetInbox != NULL && strcmp((*targetInbox).name, *buffer) != 0) {
		targetInbox = (*targetInbox).next;
	}

	//If targetInbox doesn't exist, target inbox doesn't exist
	if (targetInbox == NULL) {
		char reply[] = "ER:NEXST";
		printf("\tInbox DNE\n");
		send(client_fd, reply, (unsigned)strlen(reply) + 1, 0);
		return;
	}

	//Check if target inbox is being used
	pthread_mutex_lock(&((*targetInbox).lock));
	if ((*targetInbox).user != NULL) {
		pthread_mutex_unlock(&((*targetInbox).lock));
		char reply[] = "ER:OPEND";
		printf("\tInbox busy\n");
		send(client_fd, reply, (unsigned)strlen(reply) + 1, 0);
		return;
	}
	(*targetInbox).user = client_fd;
	pthread_mutex_unlock(&((*targetInbox).lock));

	//If already has an inbox open, close to let others have access before changing to new one
	if (*currentInbox != NULL) {
		pthread_mutex_lock(&((**currentInbox).lock));
		(**currentInbox).user = 0;
		pthread_mutex_unlock(&((**currentInbox).lock));
	}

	//Sets target inbox to current and replies to client
	*currentInbox = targetInbox;
	printf("\tOpened inbox\n");
	char reply[] = "OK!";
	send(client_fd, reply, (unsigned)strlen(reply) + 1, 0);

	return;
}
//utility function to create new empty inbox struct
struct inbox* createBox(char** buffer, size_t* bufferSize)
{
	struct inbox* messageBox = (struct inbox*)malloc(sizeof(struct inbox));
	messageBox->message1 = NULL;
	messageBox->name = *buffer;
	messageBox->next = NULL;

	//reset buffer
	*buffer = NULL;
	*bufferSize = 0;

	//if there are no Inboxes
	if (inbox1 == NULL){
		inbox1 = messageBox;
		return messageBox;
	} 
	struct inbox* head = inbox1;
	
	//append the end of the inbox list
	while(head->next!=NULL){
		head = head->next;
	}
	
	head->next = messageBox;

	return messageBox;
}

void CREAT(int client_fd, char** buffer, size_t* bufferSize)
{
	printf("Good, attempting to create a box\n");
	
	//Read length of box name
	int messageSize = getLengthFromMessage(client_fd, buffer, bufferSize);
	//error for length of name of messagebox
	//must also start with alphabetic character
	if (messageSize-1>25 || messageSize-1<5){	
		char reply [] = "ER:WHAT?";
		send(client_fd,reply,(unsigned)strlen(reply)+1,0);
		return;
	}
	

	printf("\tInbox name size is %d\n", messageSize);
	
	//Read box name from client message
	if (readNBytes(client_fd, buffer, bufferSize, messageSize) == 1) {
		printf("\tProblem readingNBytes\n");
		return;
	}
		
	struct inbox* target = inbox1;
	while (target != NULL){
		printf("Comparing %s with %s\n", (*target).name, *buffer);
		if (strcmp((*target).name, *buffer) == 0){
			//ERROR: box name exists already
			char reply[] = "ER:EXIST";
			send(client_fd, reply, (unsigned)strlen(reply)+1, 0);
			printf("\tCreate failed: box name is used\n");
			return;
		}
	
		target = (*target).next;
	}
	
	printf("\tCreating: %s\n", *buffer);
	createBox(buffer, bufferSize);
	printf("Created Box\n");
	char reply[] = "OK!";
	send(client_fd, reply, (unsigned)strlen(reply)+1,0);	 
	return;
}
void DELBX(int client_fd, char** buffer, size_t* bufferSize, struct inbox** currentInbox, struct message** currentMsg)
{
	printf("Good, attempting to delete a box\n");
	return;
}
void CLSBX(int client_fd, char** buffer, size_t* bufferSize, struct inbox** currentInbox, struct message** currentMsg)
{
	printf("Good, attempting to close a box\n");
	return;
}

//utility function to dequeue message FIFO algorithm
char* dequeue(struct inbox** currentInbox, char** buffer, size_t* bufferSize){
	
	struct message* temp = (*currentInbox)->message1;
	char* mess = temp->message;
	
	(*currentInbox)->message1 = (*currentInbox)->message1->next; 
	
	
	free(temp);
	
	return mess;
}
void NXTMG(int client_fd, char** buffer, size_t* bufferSize, struct inbox** currentInbox, struct message** currentMsg)
{
	printf("Good, attempting to get next message\n");
	
	//no box is open
	if (*currentInbox==NULL){
		char reply[] = "ER:NOOPN";
		printf("No message box is open to get the next message from\n");
		send(client_fd,reply,(unsigned)strlen(reply)+1,0);
		return;

	}

	pthread_mutex_lock(&((*currentInbox)->lock));
	//box not open
	if((*currentInbox)->user == 0){
		pthread_mutex_unlock(&((*currentInbox)->lock));
		char reply[] = "ER:NOOPN";
		printf("No message box is open\n");
		send(client_fd, reply, (unsigned)strlen(reply)+1, 0);
		return;

	}
 	//is this how to use the mutex locking 
	pthread_mutex_unlock(&((*currentInbox)->lock));
	
	//there are no messages in the inbox
	if ((*currentInbox)->message1 == NULL){
		char reply[] = "ER:EMPTY";
		printf("No message in inbox\n");
		send(client_fd, reply, (unsigned)strlen(reply)+1,0);	
		//is this right under this?
		*buffer = NULL;
		*bufferSize = 0;
		return;
	}

	printf("Attaining message\n");
	char* removedMessage = dequeue(currentInbox, buffer, bufferSize);
	printf("The message: %s, was removed", removedMessage);

	return;

}

//utility function to enqueue a message
void enqueue(struct inbox** currentInbox, char** buffer, size_t* bufferSize){
	//create the message node
	struct message* temp;
	temp =(struct message*)malloc(sizeof(struct message));
	temp -> message = *buffer;

	//there are no messages in the inbox
	if ((*currentInbox)->message1 == NULL){
		(*currentInbox)->message1 = temp; 
		temp->next = NULL;
		*buffer = NULL;
		*bufferSize = 0;
		return;
	}
	struct message* pointer = (*currentInbox)->message1;
	//append to end of message queue
	while(pointer->next != NULL) {
		pointer = pointer->next;	
	}
	pointer->next = temp;
	
	//reset buffer
	*buffer = NULL;
	*bufferSize = 0;
	
	return;

}

//check open box mutex
void PUTMG(int client_fd, char** buffer, size_t* bufferSize, struct inbox** currentInbox)
{
	printf("Good, attempting to place new message\n");

	//Read length of box name
	int messageSize = getLengthFromMessage(client_fd, buffer, bufferSize);

	printf("\tMessage size is %d\n", messageSize);

	//Read box name from client message
	if (readNBytes(client_fd, buffer, bufferSize, messageSize) == 1) {
		printf("\tProblem readingNBytes\n");
		return;
	}

	//no box is open
	if (*currentInbox == NULL){
		char reply[] = "ER:NOOPN";
		printf("No message box is open\n");
		send(client_fd, reply, (unsigned)strlen(reply)+1, 0);
		return;
	}
	
	pthread_mutex_lock(&((*currentInbox)->lock));
	//box not open
	if((*currentInbox)->user == 0){
		pthread_mutex_unlock(&((*currentInbox)->lock));
		char reply[] = "ER:NOOPN";
		printf("No message box is open\n");
		send(client_fd, reply, (unsigned)strlen(reply)+1, 0);
		return;

	}
 	//is this how to use the mutex locking 
	pthread_mutex_unlock(&((*currentInbox)->lock));

	printf("Placing Message\n");
	enqueue(currentInbox,buffer,bufferSize);
	printf("Placed Message\n");	
	
	char reply[] = "OK!";
	send(client_fd, reply, (unsigned)strlen(reply)+1,0);	
	return;
}

int getCommand(int client_fd, char** buffer, size_t* bufferSize)
{
	ssize_t readBytes = 0;
	ssize_t bytes = 6;

	if (bufferSize > 0) memset(*buffer, '\0', *bufferSize);
	*buffer = realloc(*buffer, bytes);

	printf("Awaiting command of %d\n", client_fd);

	//Reads message until no more bytes found or 6 bytes read
	//5 bytes is the command length + 1 \0

	do {
		readBytes = read(client_fd, (*buffer) + (6 - bytes), bytes);
		bytes -= readBytes;
		printf("\tRead %d bytes\n", readBytes);
	} while (bytes > 0 && readBytes > 0);

	(*buffer)[6 - bytes - 1] = '\0'; //sets char after command to \0
	if (bytes > 0) {
		return 1; //command is too short
	}
	if (readBytes < 0) {
		return 2; //error
	}

	//Read 6 bytes successfully
	//Note: read cursor is left at char 2 spaces after command, right after \0 or !
	return 0;
}


void* handleClient(void* args)
{
	//Detaches threat to make it deallocate automatically on exit
	pthread_detach(pthread_self());

	int client_fd = *(int*)args; 
	printf("Handling %d\n", client_fd);
	struct inbox* currentInbox = NULL;
	struct message* currentMsg = NULL;

	//Initialize buffer
	char* buffer = NULL;
	size_t bufferSize = 0;

	while(1) {
		//Reads message command into buffer
		int e = getCommand(client_fd, &buffer, &bufferSize);
		printf("\t%d says %s\n", client_fd, buffer);

		if (e == 1) {
			printf("\tCommand too small\n");
			//read all bytes, but not enough bytes for a command

			char* reply = "ER!WHAT?";
			send(client_fd, reply, sizeof(char) * ((unsigned)strlen(reply)+1), 0);
		} else if (e == 2) {
			printf("\tError reading command\n"); //not sure when this happens
			break;
		} else {
			//Check what command is inputted
			if (strcmp(buffer, "HELLO") == 0) {
				HELLO(client_fd, &buffer, &bufferSize);
			} else if (strcmp(buffer, "GDBYE") == 0) {
				GDBYE(client_fd, &buffer, &bufferSize);
			} else if (strcmp(buffer, "OPNBX") == 0) {
				OPNBX(client_fd, &buffer, &bufferSize, &currentInbox, &currentMsg);
			} else if (strcmp(buffer, "CREAT") == 0){
				CREAT(client_fd, &buffer, &bufferSize);
			} else if (strcmp(buffer, "NXTMG") == 0){
				NXTMG(client_fd, &buffer, &bufferSize, &currentInbox, &currentMsg);
			} else if (strcmp(buffer, "PUTMG") == 0){
				PUTMG(client_fd, &buffer, &bufferSize, &currentInbox);
			} else if (strcmp(buffer, "CLSBX") == 0){
				CLSBX(client_fd, &buffer, &bufferSize, &currentInbox, &currentMsg);
			} else if (strcmp(buffer, "DELBX") == 0){
				DELBX(client_fd, &buffer, &bufferSize, &currentInbox, &currentMsg);
			}  
		}
	}

	//Not reached
	if (buffer != NULL) free(buffer);
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
