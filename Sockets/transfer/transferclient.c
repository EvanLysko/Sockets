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

    if(argc < 3) {//they do not provide the correct amount of arguments
       error("Argument missing\n");
    }

    int sockfd, newsockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;//used to store info about a host (ie how we get info for local server)
    char *buffer = NULL;
    

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

    FILE *f;
    long filelen;

    f = fopen("text.txt", "r");
    if(f != NULL)
    {
        if (fseek(f, 0L, SEEK_END) == 0) {//go to end of file
            filelen = ftell(f);
            if (filelen < 0){//get length of file
                error("Error telling f position\n");
            }
            rewind(f);//move f to beginning

            //allocate mem to filelen
            buffer = (char *) malloc(sizeof(char) * (filelen + 1));
            
            //read file into buffer
            fread(buffer, sizeof(char), filelen, f);
            printf("%ld\n", filelen);
            printf("%s\n", buffer);
        }
    }
    
    int bytes;
    int bytes_sent;
    //send the size
    if (bytes_sent = write(sockfd, &filelen , sizeof(filelen)) > -1) {
        printf("%d bytes sent\nLength has been succesfully transfered\n", bytes_sent);
    }
    else {
        error("Error transfering length\n");
    }
    bytes_sent = 0;
    //send the file
     while( bytes_sent < filelen) {//write until error or bytes_sent == filelen
        bytes = write(sockfd, buffer, filelen);
        bytes_sent += bytes;
        printf("%d  out ot %ld bytes sent\n", bytes_sent, filelen);
    }
    if (bytes < 0) {//if error closed loop
        error("Error transfering file\n");
    }
    else {//reached filelen
    printf("Sent Full File\n");
    }

    fclose(f);
    free(buffer);
    close(sockfd);
    return 0;

}



