#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>


void error(const char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char * argv[]) {

    if(argc < 3) {//they do not provide the correct amount of arguments
       error("Argument missing\n");
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;//used to store info about a host (ie how we get info for local server)
    char *buffer = NULL;
    int filelen;
    

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
    //set up and connected -- start specific application

    //get question from server
    buffer = (char *) malloc(sizeof(char) * 80);
    int bytes = 0;
    int bytes_recv = 0; //might need to change this to -4 for some reason
    while ( bytes_recv < 80) {//read until error or bytes_sent == len buffer
        bytes = read(sockfd, buffer + bytes_recv, 80);
        bytes_recv += bytes;
        printf("%d out of 80 bytes recv\n", bytes_recv);
    }
    if (bytes < 0) {//if error closed loop
        error("Error receiving question");
    }
    else {//reached end
        printf("Server:\n%s\n", buffer);
    }
    int bytes_sent;

    //send reponse from stdin
    free(buffer);
    buffer = (char *) calloc(10, sizeof(char));
    fgets(buffer, 10, stdin);//take reply from server and put in buffer
    bytes_sent = 0;
    //send the answer
     while( bytes_sent < 10) {//write until error or bytes_sent == len buffer
        bytes = write(sockfd, buffer, 10);
        bytes_sent += bytes;
        printf("%d out of 10 bytes sent\n", bytes_sent);
    }
    if (bytes < 0) {//if error closed loop
        error("Error transfering answer\n");
    }
    else {//reached filelen
    printf("Sent Full answer\n");
    }

    FILE* fp;

    //put stdin answer as the variable for fopen
    fp = fopen("testtransfer.txt", "w");

    //get filelen
    if (bytes_recv = read(sockfd, &filelen, sizeof(filelen)) < 0) {
        error("Error receiving length");
    }
    else {
        printf("%d bytes recv\n%d\nFile length recieved\n", bytes_recv, filelen);
        free(buffer);
        buffer = (char *) calloc(filelen, sizeof(char) * (filelen));
    }

    bytes_recv = -4;//might need to change to -4
    //getfile
    while ( bytes_recv < filelen) {//write until error or bytes_sent == filelen
        bytes = read(sockfd, buffer + bytes_recv, filelen);
        bytes_recv += bytes;
        printf("%d out of %d bytes recv\n", bytes_recv, filelen);
    }
    if (bytes < 0) {//if error closed loop
        error("Error receiving file");
    }
    else {//reached filelen
        printf("%s\n", buffer);
        fwrite(buffer, 1, filelen, fp);//write buffer to file
    }
    
    fclose(fp);
    free(buffer);
    close(sockfd);
    return 0;


    
    
}