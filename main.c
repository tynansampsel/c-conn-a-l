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
    waitForMessage(&requestIp);

    printf("recieved from %s\n", requestIp);
    
    char gotLetter[3];
    printf("DO YOU WANT TO GIVE A COOKIE? :");
    fgets(gotLetter, 3, stdin);
    char *requestIpPtr = requestIp;
    sendMessage(requestIpPtr, gotLetter);

    return 0;
}



int waitForMessage(char requestIp[]){
    int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	struct sockaddr_storage their_addr;
	char buf[100];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

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
          

        //freeaddrinfo(servinfo);

        addr_len = sizeof their_addr;
        numbytes = recvfrom(sockfd, buf, 99 , 0, (struct sockaddr *)&their_addr, &addr_len);

        printf("listener: got packet from %s\n",
                inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr),
                    s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);

        int i = inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), requestIp, sizeof requestIp);
        if (i == 0) {
            printf("problem printing ip: %s\n", strerror(errno));
        }


        freeaddrinfo(servinfo);
        close(sockfd);
    }
    return 0;
}

int sendMessage(char ip[], char message[]){
    int sockfd;
	int numbytes;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints); // Initialize hints
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int a = getaddrinfo(ip, "4950", &hints, &servinfo);
    if(a == 0){
        printf("got something!!!\n");

        for(p = servinfo; p != NULL; p = p->ai_next){
            sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
            if(sockfd != -1){
                break;
                printf("found socket\n");
            }
        }
    } else {
        printf("womp womp\n");
    }


    if (p == NULL) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }

    numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
    if (numbytes == -1) {
        printf("n\n");

		perror("talker: sendto");
		exit(1);
	} else {
        printf("y\n");

    }


    freeaddrinfo(servinfo);
    close(sockfd);
    printf("Sent!\n");
}