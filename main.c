#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


void *get_in_addr(struct sockaddr *sa)
{
	return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(void)
{
    char requestIp[INET6_ADDRSTRLEN];
    if (waitForMessage(requestIp) != 0) {
        fprintf(stderr, "Error occurred while waiting for message\n");
        return 1; // Return an error code
    }

    printf("received from %s\n", requestIp);
    
    char gotLetter[3];
    printf("do you want to give them a cookie? (Y/N):");
    fgets(gotLetter, sizeof(gotLetter), stdin);

    char *requestIpPtr = requestIp;

    if (sendMessage(requestIpPtr, gotLetter) != 0) {
        fprintf(stderr, "Error occurred while sending message\n");
        return 1; // Return an error code
    }

    return 0;
}



int waitForMessage(char *requestIp){
    printf("waiting for someone to ask for a cookie...\n");

    int sockfd;
	struct addrinfo hints, *servinfo, *p;

	struct sockaddr_storage their_addr;
	char buf[100];
	socklen_t addr_len;
	char pseudoRequestIp[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int r = getaddrinfo(NULL, "4950", &hints, &servinfo);
    if (r == 0)
    {
        for(p = servinfo; p != NULL; p = p->ai_next){
            sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);

            if((sockfd) > -1){

                int b = bind(sockfd, p->ai_addr, p->ai_addrlen);

                break;
            }
            printf("nah\n");
        }
        

        addr_len = sizeof their_addr;

        if (inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), pseudoRequestIp, INET6_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
            return 1;
        } else {
            strcpy(requestIp, pseudoRequestIp);
        }


        freeaddrinfo(servinfo);
        close(sockfd);
    }
    return 0;
}

int sendMessage(char ip[], char message[]){
    int sockfd;
	int sendV;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints); // Initialize hints
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int a = getaddrinfo(ip, "4950", &hints, &servinfo);
    if(a == 0){
        for(p = servinfo; p != NULL; p = p->ai_next){
            sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
            if(sockfd != -1){
                break;
                printf("found socket\n");
            }
        }
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }

    sendV = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
    if (sendV == -1) {
		perror("talker: sendto");
		exit(1);
	}


    freeaddrinfo(servinfo);
    close(sockfd);

}