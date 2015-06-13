/******
	This is the server that receive hexadecimal number from client.
	Gcc Command Format: ./a.out [ip address] [port]
	For example: gcc server.c
		         ./a.out 127.0.0.1 2012
******/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	if (argc < 3) {
		perror("No Enough Arguments to Continue");
		exit(EXIT_FAILURE); 
	}
		
	struct sockaddr_in server; 
	memset(&server, 0, sizeof(server));
	socklen_t fromlen = sizeof(server);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(atoi(argv[2]));

	// Stream Socket
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == bind(sock, (struct sockaddr *)&server, sizeof(server))) {
		perror("Bind Failed");
		close(sock);
		exit(EXIT_FAILURE);
	} else {
		puts("Bind Succeed");
	}
	
	int numRecv = 0;
	int receiveCount = 0;
	ssize_t recSize = 0;
	while (1) {
		puts("Receive Test ...");
		recSize = recvfrom(sock, (unsigned char*)&numRecv, sizeof(char), 0, (struct sockaddr *)&server, &fromlen);
		if (recSize < 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			printf("Receive Succeed, receiveCount: %d\n", ++receiveCount);
		}
		printf("\tReceive Size : %d\n ", recSize);
		printf("\tReceive Number: %x\n", numRecv);
	}
	
	return 0;
}
