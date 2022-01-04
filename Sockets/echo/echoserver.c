#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void error(const char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char * argv[]) {

    if(argc < 2) {//they do not provide the correct amount of arguments
       error("Port not provided. Program terminated\n");
    }

    int sockfd, newsockfd, portno, n;
    struct sockaddr_in serv_addr , cli_addr;
    socklen_t clilen;
    char buffer[256];

    

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("Error opening socket\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    n = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if ( n < 0) {
        error("Binding failed\n");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if(newsockfd < 0) {
        error("Error Accepting\n");
    }

    while (1) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 256);
        if (n < 0) {
            error("Error reading\n");
        }
        printf("Client: %s\n", buffer);
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);//take reply from server and put in buffer

        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0 ) {
            error("Error writing\n");
        }

        int i = strncmp("bye", buffer, 3);
        if (i == 0) {//bye closes socket
            break;
        }
        
    }

    close(newsockfd);
    close(sockfd);
    return 0;


    
    
}