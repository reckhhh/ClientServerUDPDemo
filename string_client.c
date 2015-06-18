/******
	This is the client that send hexadecimal number to server every 180ms.

	Gcc Command Format: ./a.out [ip address] [port] [string to send]
	For example: gcc client.c
		         ./a.out 127.0.0.1 2012 "abc\x71\x5a\r\n"


	Attention: The last argument [string to send] should be coverd by "",
			   or, '\' will be missing
******/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

bool isValidHexNum (char ch) {
	if ( isdigit(ch) || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F') ) 
		return true;
	else
		return false;
}

bool isValidEscapeChar (char ch) {
	if ( ch == 'x'  || ch == 'r'  || ch == 'n'  || ch == '\'' || ch == '"'  || ch == '\\' || ch == '\0' )
	    return true;
	else
		return false;
}

int main (int argc, char *argv[]) 
{		
	if (argc < 4) {
		puts("Not Enough Arguments to Continue");
		exit(EXIT_FAILURE);	
	}

	// Datagram Client
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock) {
		puts("Creating Socket Error");
		exit(EXIT_FAILURE);
	} else {
		puts("Creating Socket Succeed");
	}
 
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr(argv[1]);
	client.sin_port = htons(atoi(argv[2]));
	
	// Send Number
	char str[1024];
	memset(&str, 0, sizeof(str));
	strcpy(str, argv[3]);
	printf("str = %s\n\n", str);
	
	char buffer[1024];
	memset(&buffer, 0, sizeof(buffer));

	int i = 0;
	int j = 0;
	
	while (i <= strlen(str)) 
	{
		if (str[i] != '\\') {
			buffer[j++] = str[i];
            ++i;
        }
        else if ( str[i] == '\\' && isValidEscapeChar(str[i + 1]) ) {
			switch(str[i + 1]) {
				case 'x': i += 2;
						  if ( isValidHexNum(str[i]) && isValidHexNum(str[i + 1]) ) {
							  char ptr[3];
							  ptr[0] = str[i];
							  ptr[1] = str[i + 1];
							  ptr[2] = '\0';
							  
							  buffer[j++] = strtol(ptr , NULL, 16);
							  i += 2;
						  } else {
							  buffer[j++] = str[i];
							  ++i;
						  }
						  break;
						  
				 case 'r': buffer[j++] = '\r';
				           i += 2;
                           break;
                              
                 case 'n': buffer[j++] = '\n';
                           i += 2;
                           break;
                              
                 case '\'': buffer[j++] = '\'';
                            i += 2;
                            break;
                               
                 case '"': buffer[j++] = '"';
                           i += 2;
                           break;
                              
                 case '\\': buffer[j++] = '\\';
                            i += 2;
                            break;
                               
                 case '\0': buffer[j++] = '\0';
							i += 2;
                            break;
                              
                 default : i += 2;
                           break;
            }   
        }
        else if (str[i] == '\\' && isdigit(str[i + 1])) {
			buffer[j++] = str[i + 1];
			i += 2;
		}
	    else {
			buffer[j++] = str[i];
			++i;
		}
    }
    buffer[j] = '\0';
    	
    	
    int sendCount = 0;
    while(1) 
    {	
    	usleep(180000);
    		
   		// sendto(int socket, char data, int dataLength, flags, destinationAddress, int destinationStructureLength)
    	int byteSent = sendto(sock, buffer, strlen(buffer)+1, 0, (struct sockaddr*)&client, sizeof(struct sockaddr_in));
    	if (byteSent < 0) {
    		printf("Sending Error: %s\n", strerror(errno)); 
    		exit(EXIT_FAILURE);
    	} else {
    		printf("Sending %s Succeed, sendCount: %d\n", buffer, ++sendCount);
        }
    }
    
	close(sock);
	return 0;
}
