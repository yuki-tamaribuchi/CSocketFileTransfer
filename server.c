#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h> // for IPPROTO_TCP
#include<string.h> // for memset?
#include<fcntl.h> // for open
#include<unistd.h> // for close

#define TRUE 0
#define FALSE -1

#define PORT_USED 60000


struct file_struct{
	char file_name[255];
	long file_size;
	char file_buf[100000]; //1000KB
	
};


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

int accept_connection(int sock){
	int new_sock;
	int addr_len;
	struct sockaddr_in writer_sin;

	memset(&writer_sin, 0, sizeof(struct sockaddr_in));
	writer_sin.sin_family = AF_INET;
	writer_sin.sin_port = htons(PORT_USED);
	writer_sin.sin_addr.s_addr = htons(INADDR_ANY);

	addr_len = sizeof(writer_sin);

	while(1){
		if((new_sock=accept(sock, (struct sockaddr*)&writer_sin, (socklen_t*)&addr_len))<0){
			perror("Failed to accept\n");
			return FALSE;
		}
		return new_sock;
	}
}

int recieve_file(int sock, struct file_struct* recv_file_struct){
	char buff[1000271];
	long file_size;

	if((file_size=recv(sock, (void*)buff, sizeof(buff), 0))<0){
		perror("Failed to recv\n");
		close(sock);
		return FALSE;
	}
	memcpy((void*)recv_file_struct, (const void*)buff, file_size);

	return TRUE;
}

int save_file(struct file_struct* recv_file_struct){
	FILE* dest_file_ptr;
	char file_name[255], file_buf[1000000];
	long file_size;

	strncpy(file_name, recv_file_struct->file_name, 255);
	file_size = recv_file_struct->file_size;
	memcpy((void*)file_buf, (void*)recv_file_struct->file_buf, file_size);
	printf("File name: %s\n", file_name);
	printf("File size: %d\n", file_size);
	

	if((dest_file_ptr = fopen(file_name, "w+"))==NULL){
		perror("Failed to fopen()\n");
		return FALSE;
	}

	if(fwrite(file_buf, file_size, 1, dest_file_ptr)<1){
		perror("Failed to fwrite()\n");
		return FALSE;
	}

	if(fclose(dest_file_ptr)==EOF){
		perror("Failed to fclose()\n");
		return FALSE;
	}

	return TRUE;
}


int main(){
	int sock, new_sock;
	struct file_struct recv_file_struct;

	memset((void*)&recv_file_struct, 0, sizeof(struct file_struct));

	if((sock = open_and_bind_socket())<0){
		perror("Failed to open_and_bind_socket()\n");
		return FALSE;
	}
	printf("Socket opened\n");

	if(do_listen(sock)<0){
		perror("Failed to do_listen()\n");
		close(sock);
		return FALSE;
	}
	printf("Listen socket\n");

	if((new_sock = accept_connection(sock))<0){
		perror("Failed to accept_connection()\n");
		close(sock);
		return FALSE;
	}
	printf("New socket opened\n");

	if(recieve_file(new_sock, &recv_file_struct)<0){
		perror("Failed to recieve_file()\n");
		close(new_sock);
		return FALSE;
	}
	printf("File recieved\n");

	if(save_file(&recv_file_struct)<0){
		perror("Failed to save_file()\n");
		return FALSE;
	}
	printf("File saved\n");
	
	close(sock);
	close(new_sock);
	return TRUE;
}