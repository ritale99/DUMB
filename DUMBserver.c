#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 

int main(int argc, char * argv[]){
	
	int server_fd, newSocket; 
	char buffer [1024]; 
	struct sockaddr_in address; 
	struct sockaddr_storage serverStorage;
	socklen_t addr_size; 

	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	
	address.sin_family = AF_INET;
	
	address.sin_port = htons(atoi(argv[2])); 
	
	address.sin_addr.s_addr = inet_addr(atoi(argv[1]));

	memset(address.sin_zero,'\0' ,sizeof address.sin_zero);

	bind (server_fd, (struct sockaddr *) &address, sizeof(address));
	
	if (listen(server_fd, 5)==0){
		printf("Listening\n");
			
	}
	
	else{
		printf("Error\n");	
	}
	
	addr_size - sizeof serverStorage;
	newSocket = accept(server_fd, (struct sockaddr *) &serverStorage, &addr_size);

	strcpy(buffer,"MSG");
	send(newSocket, buffer, 13, 0);

	return 0; 

}
