#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>

#define TRUE 0
#define FALSE -1

#define PORT_USED 60000


int open_and_bind_socket(){
	int sock;
	struct sockaddr_in writer_sin;

	memset((void*)&writer_sin, 0, sizeof(struct sockaddr_in));
	writer_sin.sin_family = AF_INET;
	writer_sin.sin_port = htons(PORT_USED);
	writer_sin.sin_addr.s_addr = htons(INADDR_ANY);

	if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		perror("Failed to open socket\n");
		return FALSE;
	}

	if(bind(sock, (struct sockaddr_in *)&writer_sin, sizeof(struct sockaddr))<0){
		perror("Failed to bind socket\n");
		close(sock);
		return FALSE;
	}

	return sock;
}


int main(int argc, char **argv){
	int sock;

	if((sock = open_and_bind_socket())<0){
		perror("Failed to open_and_bind_socket\n");
		return FALSE;
	}

	close(sock);
	return TRUE;
}