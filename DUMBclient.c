#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h> 

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

	char buffer[1024];
	char str[7];
	socklen_t addr_size;
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
	
	
	return 0; 


}
