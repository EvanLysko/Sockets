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
#include <ctype.h>


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
    //set up and connected -- start specific application

    //ask what file they want
    int bytes;
    int bytes_sent;
    buffer = (char *) malloc(sizeof(char) * 80);
    sprintf(buffer, "Enter name of the file you would like:\ntext1.txt\ntext2.txt\ntext3.txt\n");
    //send the question to client
    bytes_sent = 0;
     while( bytes_sent < 80) {//write until error or bytes_sent == len of buffer
        bytes = write(newsockfd, buffer, 80);
        bytes_sent += bytes;
        printf("%d  out ot 80 bytes sent\n", bytes_sent);
    }
    if (bytes < 0) {//if error closed loop
        error("Error transfering file\n");
    }
    else {//reached ended of message
    printf("Sent Full Message\n");
    }

    //get the answer
    free(buffer);
    buffer = (char *) calloc(10, sizeof(char));
    bytes = 0;
    char *filename;
    int bytes_recv = 0; //might need to change this to -4 for some reason
    while ( bytes_recv < 10) {//read until error or bytes_sent == 1
        bytes = read(newsockfd, buffer + bytes_recv, 10);
        bytes_recv += bytes;
        printf("%d out of 10 bytes recv\n", bytes_recv);
    }
    if (bytes < 0) {//if error closed loop
        error("Error receiving answer");
    }
    else {//reached end
        printf("they want %s\n", buffer);
    }

    //getfilelen to send to client
    FILE *f;
    long filelen;

    f = fopen(buffer, "r");
    if(f != NULL)
    {
        if (fseek(f, 0L, SEEK_END) == 0) {//go to end of file
            filelen = ftell(f);
            if (filelen < 0){//get length of file
                error("Error telling f position\n");
            }
            rewind(f);//move f to beginning

            //allocate mem to filelen
            free(buffer);
            buffer = (char *) calloc(filelen, sizeof(char));
            
            //read file into buffer
            fread(buffer, sizeof(char), filelen, f);
            printf("%ld\n", filelen);
            printf("%s\n", buffer);
        }
    }
    else{//wrong filename
        error("File does not exist\n");
    }
    
    
    //send the size
    if (bytes_sent = write(newsockfd, &filelen , sizeof(filelen)) > -1) {
        printf("%d bytes sent\nLength has been succesfully transfered\n", bytes_sent);
    }
    else {
        error("Error transfering length\n");
    }
    bytes_sent = 0;
    //send the file
     while( bytes_sent < filelen) {//write until error or bytes_sent == filelen
        bytes = write(newsockfd, buffer, filelen);
        bytes_sent += bytes;
        printf("%d  out of %ld bytes sent\n", bytes_sent, filelen);
    }
    if (bytes < 0) {//if error closed loop
        error("Error transfering file\n");
    }
    else {//reached filelen
    printf("Sent Full File\n");
    }

    fclose(f);
    free(buffer);
    close(newsockfd);
    close(sockfd);
    return 0;

}



