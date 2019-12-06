#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h> 

<<<<<<< HEAD
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
=======
int client_fd;
struct sockaddr_in clientAddress;

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

int main(int argc, char* argv[]){

>>>>>>> 3d01f7dbdc0cef5fc16b50a18b63538bad8cb8d4
	char buffer[1024];
	char str[7];
	socklen_t addr_size;
<<<<<<< HEAD
	
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
=======
	addr_size = sizeof clientAddress;
		
	setupClient(htons(atoi(argv[2])), inet_addr(argv[1]));
		

		//try to connect 3 times 
		int i;
		for(i = 0; i<3; i++){	
			if (connect(client_fd, (struct sockaddr *) &clientAddress, addr_size)>=0){
				send(client_fd,"HELLO",7,0); 	
				recv(client_fd,buffer, 1024, 0);
				printf("%s\n", buffer);
				break; 
			}
			else if (i==2){
				//connection has failed
				printf("Connection Failed\n");
				return 0;	
			}	
				
		
		}
		
		//continue entering messages
		while(1){
			printf("Enter the Message\n");
			scanf("%s",str);
			
			//sends GDBYE with no arguments
			if(strcmp(str,"quit")==0)
			{
				
			}
		//create message box: takes in: create arg0
		else if(strcmp(str, "create")==0)
			{
				printf("Create message box with what name?\n");
				continue;
			}
		
		// else if ()
		// 	{

		// 	}

		else{
			//
		}

			return;
		
			
		}
>>>>>>> 3d01f7dbdc0cef5fc16b50a18b63538bad8cb8d4
	
	
	return 0; 


}
