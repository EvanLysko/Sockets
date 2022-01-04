/*
filename server_ipaddress portno

argv[0] filname
argv[1] server_ipaddress
argv[2] portno

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void error(const char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char * argv[]) {

    if(argc < 3) {//they do not provide the correct amount of arguments
       error("Argument missing\n");
    }

    int sockfd, newsockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;//used to store info about a host (ie how we get info for local server)
    char buffer[256];

    

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("Error opening socket\n");
    }

    server = gethostbyname(argv[1]);//ip_address of local server
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));//clearing struct
    portno = atoi(argv[2]);

    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);// this is copying data from hostent
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error connecting\n");
    }

    while (1) {
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0) {
            error("Error writing");
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, 256);
        if (n < 0) {
            error("Error Reading");
        }
        printf("Server: %s", buffer);

        int i = strncmp("bye", buffer, 3);
        if (i == 0) {//bye closes socket
            break;
        }

    }

    close(sockfd);
    return 0;

}



