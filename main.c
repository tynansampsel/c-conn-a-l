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
          

        freeaddrinfo(servinfo);

        addr_len = sizeof their_addr;
        numbytes = recvfrom(sockfd, buf, 99 , 0, (struct sockaddr *)&their_addr, &addr_len);

        printf("listener: got packet from %s\n",
                inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr),
                    s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);

        close(sockfd);

        // SEND RESPONSE

	    char *gotLetter[3];
        printf("DO YOU WANT TO GIVE A COOKIE? :", buf);

        fgets(gotLetter, 3, stdin);


        if (gotLetter[strlen(gotLetter) - 1] == '\n') {
            gotLetter[strlen(gotLetter) - 1] = '\0';
        }

        int numbytes2;

        numbytes2 = sendto(sockfd, gotLetter, strlen(gotLetter), 0, p->ai_addr, p->ai_addrlen);
        if (numbytes2 == -1) {
            printf("n\n");

            perror("talker: sendto");
            exit(1);
        } else {
            printf("y\n");

        }

        printf("a4\n");

        printf("Sent!\n");

    }

    return 0;
}