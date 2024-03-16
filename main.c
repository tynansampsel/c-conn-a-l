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
        return 1;
    }

    //printf("received from %s\n", requestIp);
    
    char answerInput[4];
    printf("do you want to give them a cookie? (Y/N):");
    fgets(answerInput, sizeof(answerInput), stdin);
    char answer[3];
    if(answerInput[0] == 'y') {
        strcpy(answer, "yes");
    } else if (answerInput[0] == 'n') {
        strcpy(answer, "no");
    } else {
        strcpy(answer, "idk");
    }

    char *requestIpPtr = requestIp;

    if (sendMessage(requestIpPtr, answer) != 0) {
        fprintf(stderr, "Error occurred while sending message\n");
        return 1;
    }

    return 0;
}



int waitForMessage(char *requestIp){
    printf("waiting for for someone to ask...\n");
    int sockfd;
	struct addrinfo addrinfoPrefs, *servinfo, *p;
	struct sockaddr_storage their_addr;
	char recievedInfo[100];
	socklen_t addr_len;
	char pseudoRequestIp[INET6_ADDRSTRLEN];

    memset(&addrinfoPrefs, 0, sizeof addrinfoPrefs);
    addrinfoPrefs.ai_socktype = SOCK_DGRAM;
    addrinfoPrefs.ai_flags = AI_PASSIVE;

    int getaddrinfoResult = getaddrinfo(NULL, "4950", &addrinfoPrefs, &servinfo);
    if(getaddrinfoResult != 0){
        printf("Could not get address information!\n");
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        if((sockfd) > -1){
            bind(sockfd, p->ai_addr, p->ai_addrlen);
            break;
        }
    }
    if (p == NULL) {
        printf("Could not create a socket!\n");
        return 1;
    }

    addr_len = sizeof their_addr;
    
    int recvfromResult = recvfrom(sockfd, recievedInfo, 99 , 0, (struct sockaddr *)&their_addr, &addr_len);
    if (recvfromResult == -1) {
		printf("Could not get socket information!\n");
        return 1;
	}
    
    printf("someone asked for a cookie!\n");

    char *requestIpResult = inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), pseudoRequestIp, INET6_ADDRSTRLEN);
    if (requestIpResult == NULL) {
        printf("Could not get senders ip!\n");
        return 1;
    }

    strcpy(requestIp, pseudoRequestIp);

    freeaddrinfo(servinfo);
    close(sockfd);
    return 0;
}

int sendMessage(char ip[], char message[]){
    int sockfd;
    struct addrinfo addrinfoPrefs, *servinfo, *p;

    memset(&addrinfoPrefs, 0, sizeof addrinfoPrefs);
    addrinfoPrefs.ai_family = AF_INET;
    addrinfoPrefs.ai_socktype = SOCK_DGRAM;

    int getaddrinfoResult = getaddrinfo(ip, "4950", &addrinfoPrefs, &servinfo);
    if(getaddrinfoResult != 0){
        printf("Could not get address information!\n");
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        if(sockfd != -1){
            break;
        }
    }

    if (p == NULL) {
        printf("Could not create a socket!\n");
        return 1;
    }

    int sendtoResult = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
    if (sendtoResult == -1) {
		printf("Could not send message!\n");
        return 1;
	}


    freeaddrinfo(servinfo);
    close(sockfd);
}