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

/* A buffer large enough to contain the longest allowed string */
#define BUFSIZE 218

#define USAGE                                                          \
  "usage:\n"                                                           \
  "  echoclient [options]\n"                                           \
  "options:\n"                                                         \
  "  -s                  Server (Default: localhost)\n"                \
  "  -p                  Port (Default: 10823)\n"                      \
  "  -m                  Message to send to server (Default: \"Hello " \
  "Summer.\")\n"                                                       \
  "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"message", required_argument, NULL, 'm'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv) {
  int option_char = 0;
  char *hostname = "localhost";
  unsigned short portno = 10823;
  char *message = "Hello Summer!!";
  

  // Parse and set command line arguments
  while ((option_char =
              getopt_long(argc, argv, "s:p:m:hx", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 's':  // server
        hostname = optarg;
        break;
      case 'p':  // listen-port
        portno = atoi(optarg);
        break;
      default:
        fprintf(stderr, "%s", USAGE);
        exit(1);
      case 'm':  // message
        message = optarg;
        break;
      case 'h':  // help
        fprintf(stdout, "%s", USAGE);
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

  if (NULL == message) {
    fprintf(stderr, "%s @ %d: invalid message\n", __FILE__, __LINE__);
    exit(1);
  }

  if (NULL == hostname) {
    fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
    exit(1);
  }

  /* Socket Code Here */
  int status, sad_socket, etest, bytes_received, bytes_sent, len;
  char portstr[INET6_ADDRSTRLEN];
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results
  
  /*if (argc != 3) {
        fprintf(stderr,"please provide host, port, and message\n");
        exit(1);
    }*/

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  //hints.ai_flags = AI_PASSIVE; // fill in ip 

  // get ready to connect

  sprintf(portstr, "%d", portno);

  status = getaddrinfo(hostname, portstr, &hints, &servinfo);
  if (status < 0 ) perror("ERROR getaddrinfo");

  sad_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (sad_socket < 0) perror("ERROR creating socket\n");
  
  //connect
  etest = connect(sad_socket, servinfo->ai_addr, servinfo->ai_addrlen);
  if (etest < 0) perror("ERROR connecting socket\n");

  freeaddrinfo(servinfo); // free the linked list

  //send message to server
  len = strlen(message);
  bytes_sent = send(sad_socket, message, len, 0);
  if (bytes_sent < 0) perror("ERROR echoing message to client");

  //recieve echo back from server and print it
  bzero(message, 16);
  bytes_received = recv(sad_socket, message, 16, 0);
  if (bytes_received < 0) perror("ERROR recieving message from client");
  printf(message);

  return 0;
  
}
