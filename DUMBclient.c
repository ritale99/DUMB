#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h> 

int main(int argc, char* argv[]){

	int clientSocket;
	char buffer[1024];
	struct sockaddr_in address;
	socklen_t addr_size;
	
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET; 
	address.sin_port = htons(atoi(argv[2]));
	address.sin_addr.s_addr = inet_addr(atoi(argv[1])); 
	
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	addr_size = sizeof address;
	
	connect(clientSocket, (struct sockaddr *) &address, addr_size);
	recv(clientSocket, buffer, 1024, 0 ); 

	printf("Message is: %s", buffer); 
	
	return 0; 


}
