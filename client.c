/******
	This is the client that send hexadecimal number to server every 180ms.

	Gcc Command Format: ./a.out [ip address] [port] [string to send]
	For example: gcc client.c
		         ./a.out 127.0.0.1 2012 "abc\x12\x22\r\n"


	Warning: The last argument [string to send] should be coverd by "",
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

    while(1) 
    {
        int i = 0;
        int numToSend = 0;
        
        while (i <= strlen(str))
        {
            if (str[i] != '\\') {
                numToSend = str[i];
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
                                
                                    numToSend = strtol(ptr , NULL, 16);
                                    i += 2;
                			  } else {
                                    puts("Error, Hexadecimal Number inValid");
                                    exit(EXIT_FAILURE);
                              }
                              break;
                              
                    case 'r': numToSend = '\r';
                              i += 2;
                              break;
                              
                    case 'n': numToSend = '\n';
                              i += 2;
                              break;
                              
                    case '\'': numToSend = '\'';
                               i += 2;
                               break;
                               
                    case '"': numToSend = '"';
                               i += 2;
                               break;
                              
                    case '\\': numToSend = '\\';
                               i += 2;
                               break;
                               
                    case '\0': numToSend = '\0';
							   i += 2;
                               break;
                              
                    default : i += 2;
                              break;
                }   
            }
            else if (str[i] == '\\' && isdigit(str[i + 1])) {
                numToSend = str[i + 1];
                i += 2;
            }
            else {
    			numToSend = str[i];
    			printf("str[%d] = %x    ", i, numToSend);
    			++i;
    		}
    		
			// Send numToSend to Server 
    		int sendCount = 0;
    		usleep(180000);
    		
    		// sendto(int socket, char data, int dataLength, flags, destinationAddress, int destinationStructureLength)
    		int byteSent = sendto(sock, (unsigned char*)&numToSend, sizeof(int), 0, (struct sockaddr*)&client, sizeof(client));
    		if (byteSent < 0) {
    			printf("Sending Error: %s\n", strerror(errno)); 
    			exit(EXIT_FAILURE);
    		} else {
    			printf("Sending %x Succeed, sendCount: %d\n", numToSend, ++sendCount);
            }
    	}
    }
    
	close(sock);
	return 0;
}
