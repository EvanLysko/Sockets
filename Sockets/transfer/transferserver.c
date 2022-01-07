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
    char *buffer = NULL;
    int filelen;

    

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

    FILE* fp;

    fp = fopen("recieved.txt", "w");

    int bytes;
    int bytes_recv;

    //get filelen
    if (bytes_recv = read(newsockfd, &filelen, sizeof(filelen)) < 0) {
        error("Error receiving length");
    }
    else {
        printf("%d bytes recv\n%d\nFile length recieved\n", bytes_recv, filelen);
        buffer = (char *) malloc(sizeof(char) * (filelen + 1));
    }

    bytes_recv = -4;
    //getfile
    while ( bytes_recv < filelen) {//write until error or bytes_sent == filelen
        bytes = read(newsockfd, buffer + bytes_recv, sizeof(buffer));
        bytes_recv += bytes;
        printf("%d out of %d bytes recv\n", bytes_recv, filelen);
    }
    if (bytes < 0) {//if error closed loop
        error("Error receiving file");
    }
    else {//reached filelen
        printf("%s\n", buffer);
        fwrite(buffer, 1, bytes_recv, fp);//write buffer to file
        printf("%d bytes recv\nThe file has been recieved successfully\n", bytes_recv);
    }
    
    fclose(fp);
    close(newsockfd);
    close(sockfd);
    return 0;


    
    
}