#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 218

#define USAGE                                                        \
  "usage:\n"                                                         \
  "  echoserver [options]\n"                                         \
  "options:\n"                                                       \
  "  -p                  Port (Default: 10823)\n"                    \
  "  -m                  Maximum pending connections (default: 5)\n" \
  "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"port", required_argument, NULL, 'p'},
    {"maxnpending", required_argument, NULL, 'm'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv) {
  int option_char;
  int portno = 10823; /* port to listen on */
  int maxnpending = 5;

  // Parse and set command line arguments
  while ((option_char =
              getopt_long(argc, argv, "p:m:hx", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 'p':  // listen-port
        portno = atoi(optarg);
        break;
      default:
        fprintf(stderr, "%s ", USAGE);
        exit(1);
      case 'm':  // server
        maxnpending = atoi(optarg);
        break;
      case 'h':  // help
        fprintf(stdout, "%s ", USAGE);
        exit(0);
        break;
    }
  }

  setbuf(stdout, NULL);  // disable buffering

  if ((portno < 1025) || (portno > 65535)) {
    fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__,
            portno);
    exit(1);
  }
  if (maxnpending < 1) {
    fprintf(stderr, "%s @ %d: invalid pending count (%d)\n", __FILE__, __LINE__,
            maxnpending);
    exit(1);
  }

  /* Socket Code Here */
  int status, server_socket, client_socket, etest, bytes_received, bytes_sent, len;
  char message[16];
  char portstr[INET6_ADDRSTRLEN];
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *servinfo;  // will point to the results

  /* check command line args 
  if (argc != 3) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }*/
  sprintf(portstr, "%d", portno);

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol= IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE; // fill in ip 

  // get ready to connect
  status = getaddrinfo(NULL, portstr, &hints, &servinfo);
  if (status < 0) perror("ERROR getaddrinfo");

  //create socket
  server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (server_socket < 0) perror("ERROR creating socket\n");

  //make it reuse sockets
  int yes=1;
  if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt\n");
    exit(1);
  } 

  //bind
  etest = bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen);
  if (etest < 0) perror("ERROR binding socket\n");
  

  /*connect
  etest = connect(server_socket, servinfo->ai_addr, servinfo->ai_addrlen);
  if (etest < 0) perror("ERROR connecting socket\n");*/

  freeaddrinfo(servinfo); // free the linked list

  //listen
  etest = listen(server_socket, 5);
  if (etest < 0) perror("ERROR listening\n");

  //while loop to serve multiple requests from client
  
  while(1){
    //accept connection listened to and make new socket to interact with client
    addr_size = sizeof(their_addr);
    client_socket = accept(server_socket, (struct sockaddr *) &their_addr, &addr_size);
    if (client_socket < 0) perror("ERROR accepting connection");
  
    //recieve message from client
    bzero(message, 16);
    bytes_received = recv(client_socket, message, 15, 0);
    if (bytes_received < 0) perror("ERROR recieving message from client");

    //echo client message back
    len = strlen(message);
    bytes_sent = send(client_socket, message, len, 0);
    if (bytes_sent < 0) perror("ERROR echoing message to client");
  }

  return 0;
  
}
