#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "QueryProtocol.h"

char *port_string = "1500";
unsigned short int port;
char *ip = "127.0.0.1";

#define BUFFER_SIZE 1000

void send_message(char *msg, int sock_fd) {
        write(sock_fd, msg, strlen(msg));
}

void read_response(int sock_fd) {
        char resp[BUFFER_SIZE];
        int len = read(sock_fd, resp, BUFFER_SIZE - 1);
        resp[len] = '\0';
        printf("%s\n", resp);
}


void RunQuery(char *query) {
  int address_info_result;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct addrinfo hints, *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;  // IPv4 only
  hints.ai_socktype = SOCK_STREAM;  // TCP

  address_info_result = getaddrinfo(ip, port_string, &hints, &result);

  if (address_info_result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(address_info_result));
    exit(1);
  }

  if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
    // printf("The connection was refused.\n");
    perror("connect");
    freeaddrinfo(result);
    exit(2);
  }
  freeaddrinfo(result);
  printf("Connected to movie server.\n\n");

  char ack[BUFFER_SIZE];
  int ack_len = read(sock_fd, ack, BUFFER_SIZE - 1);
  ack[ack_len] = '\0';
  if (CheckAck(ack) != 0) {
    printf("Did not receive expected ACK from server.\n");
    return;  // ACK not working properly.
  }

  send_message(query, sock_fd);  // Send the query

  int resp;  // Should be the number of responses.
  read(sock_fd, &resp, BUFFER_SIZE - 1);
  printf("Receiving %d responses\n", resp);
  SendAck(sock_fd);  // Got the response. Now initiating get all results.

  // Section responsible for obtaining all of the responses and printing them.
  for (int i = 0; i < resp; i++) {
    read_response(sock_fd);
    SendAck(sock_fd);
  }

  // Receive the goodbye
  char goodbye[BUFFER_SIZE];
  int goodbye_len = read(sock_fd, goodbye, 999);
  goodbye[goodbye_len] = '\0';
  printf("Got %d bytes. Resp: %s\n", goodbye_len, goodbye);

  if (CheckGoodbye(goodbye) == 0) {
    close(sock_fd);
  }
}

void RunPrompt() {
  char input[BUFFER_SIZE];

  while (1) {
    printf("Enter a term to search for, or q to quit: ");
    scanf("%s", input);

    printf("input was: %s\n", input);

    if (strlen(input) == 1) {
      if (input[0] == 'q') {
        printf("Thanks for playing! \n");
        return;
      }
    }
    printf("\n\n");
    RunQuery(input);
  }
}

// This function connects to the given IP/port to ensure
// that it is up and running, before accepting queries from users.
// Returns 0 if can't connect; 1 if can.
int CheckIpAddress(char *ip, char *port) {
  int address_info_result;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct addrinfo hints, *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;  // IPv4 only
  hints.ai_socktype = SOCK_STREAM;  // TCP

  address_info_result = getaddrinfo(ip, port, &hints, &result);

  if (address_info_result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(address_info_result));
    freeaddrinfo(result);
    return 0;  // Address info does not even exist.
  }

  if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
    perror("connect");
    freeaddrinfo(result);
    return 0;
  }
  freeaddrinfo(result);
  SendGoodbye(sock_fd);
  close(sock_fd);

  return 1;  // Indicates successful connection.
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Incorrect number of arguments. \n");
    printf("Correct usage: ./queryclient [IP] [port]\n");
  } else {
    ip = argv[1];
    port_string = argv[2];
  }
  if (CheckIpAddress(ip, port_string) == 1) {
    RunPrompt();
  }
  return 0;
}
