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

int do_connect(int sock, char* dest_ip_addr){
	struct sockaddr_in reader_sin;

	memset((void*)&reader_sin, 0, sizeof(struct sockaddr_in));
	reader_sin.sin_family = AF_INET;
	reader_sin.sin_port = htons(PORT_USED);
	reader_sin.sin_addr.s_addr = inet_addr(dest_ip_addr);

	if(connect(sock, (struct sockaddr_in *)&reader_sin, sizeof(struct sockaddr_in))<0){
		perror("Failed to connect\n");
		return FALSE;
	}
	return TRUE;
}


int do_send_file(int sock, char* file_path){
	FILE* file_ptr;
	long file_size;

	if((file_ptr = fopen(file_path, "r"))==NULL){
		perror("Failed to open file\n");
		close(sock);
		return FALSE;
	};

	if(fseek(file_ptr, 0, SEEK_END)<0){
		perror("Failed to fseek() SEEK_END\n");
		close(sock);
		return FALSE;
	}

	if((file_size = ftell(file_ptr))<0){
		perror("Failed to ftell()\n");
		close(sock);
		return FALSE;
	}

	if(fseek(file_ptr, 0, SEEK_SET)<0){
		perror("Failed to fseek() SEEK_SET\n");
		close(sock);
		return FALSE;
	}

	if(send(sock, (const void*)file_ptr, file_size, 0)<0){
		perror("Failed to send()\n");
		close(sock);
		return FALSE;
	}

	return TRUE;
}


int main(int argc, char **argv){
	char file_path[200], dest_ip_addr[16];
	int sock;

	if(argc<3){
		printf("Pass file path and destination ip address as commandline argument\n");
		return FALSE;
	}

	if(sscanf(argv[1], "%s", &file_path)!=1){
		perror("Failed to sscanf() file_path\n");
		return FALSE;
	}

	printf("File path is %s\n", file_path);

	if(sscanf(argv[2], "%s", &dest_ip_addr)!=1){
		perror("Failed to sscanf() dest_ip_addr\n");
		return FALSE;
	}

	printf("Destination ip address is %s\n", dest_ip_addr);

	if((sock = open_and_bind_socket())<0){
		perror("Failed to open_and_bind_socket\n");
		return FALSE;
	}

	if(do_connect(sock, &dest_ip_addr)<0){
		perror("Faild to do_connect\n");
		return FALSE;
	}

	if(do_send_file(sock, &file_path)<0){
		perror("Failed to do_send_file()\n");
		return FALSE;
	}
	
	close(sock);
	return TRUE;
}