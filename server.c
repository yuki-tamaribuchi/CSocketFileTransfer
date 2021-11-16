#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h> // for IPPROTO_TCP
#include<string.h> // for memset?
#include<fcntl.h> // for open
#include<unistd.h> // for close

#define TRUE 0
#define FALSE -1

#define PORT_USED 60000


int open_and_bind_socket(){
	int sock;
	struct sockaddr_in reader_sin;

	memset((void*)&reader_sin, 0, sizeof(struct sockaddr_in));
	reader_sin.sin_family = AF_INET;
	reader_sin.sin_port = htons(PORT_USED);
	reader_sin.sin_addr.s_addr = htons(INADDR_ANY);

	if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		perror("Failed to open socket\n");
		return FALSE;
	}

	if((bind(sock, (struct sockaddr*)&reader_sin, sizeof(struct sockaddr_in)))<0){
		perror("Failed to bind socket\n");
		return FALSE;
	}
	return sock;
}

int do_listen(int sock){
	if((listen(sock, 1))<0){
		perror("Failed to listen\n");
		return FALSE;
	}
	return TRUE;
}

int main(){
	int sock;

	if((sock = open_and_bind_socket())<0){
		perror("Failed to open_and_bind_socket()\n");
		return FALSE;
	}

	if(do_listen(sock)<0){
		perror("Failed to do_listen()\n");
		close(sock);
		return FALSE;
	}

	close(sock);
	return TRUE;
}