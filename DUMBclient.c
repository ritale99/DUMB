#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h> 

int attemptConnect(int clientSocket, struct sockaddr* addressPtr, int addr_size, char* buffer)
{
	int i;
	//3 attempts to connect
	for (i = 0; i < 3; i++) {
		//Attempt connect
		if (connect(clientSocket, addressPtr, addr_size) >= 0) {
			send(clientSocket, "HELLO", 7, 0);
			recv(clientSocket, buffer, 1024, 0);
			printf("%s\n", buffer);
			return 0;
		}
	}
	printf("Connection failed\n");
	return -1;
}

int main(int argc, char* argv[])
{
	int clientSocket;
	char buffer[1024];
	char str[7];
	struct sockaddr_in address;
	socklen_t addr_size;
	
	//Setup socket to connect to server
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET; 
	address.sin_port = htons(atoi(argv[2]));
	address.sin_addr.s_addr = inet_addr(argv[1]); 
	
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	memset(buffer, '\0', sizeof(char)*1024);
	addr_size = sizeof address;
	
	//Attempt to connect to server
	if (attemptConnect(clientSocket, (struct sockaddr*)&address, addr_size, buffer) < 0) {
		return 0;
	}
		
	//continue entering messages
	while(1) {
		printf("Enter the Message\n");
		scanf("%s",str);

		//switch(str)

		//send to server...


		//receive from server...

	}
	
	recv(clientSocket, buffer, 1024, 0 ); 

	//printf("Message is: %s", buffer); 
	
	return 0; 


}
