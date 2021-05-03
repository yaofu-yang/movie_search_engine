#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include "QueryProtocol.h"
#include "MovieSet.h"
#include "MovieIndex.h"
#include "DocIdMap.h"
#include "Hashtable.h"
#include "QueryProcessor.h"
#include "FileParser.h"
#include "DirectoryParser.h"
#include "FileCrawler.h"
#include "Util.h"

#define BUFFER_SIZE 1000

int Cleanup();

DocIdMap docs;
MovieTitleIndex docIndex;

#define SEARCH_RESULT_LENGTH 1500

char movieSearchResult[SEARCH_RESULT_LENGTH];

void sigchld_handler(int s) {
  write(0, "Handling zombies...\n", 20);
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}


void sigint_handler(int sig) {
  write(0, "Ahhh! SIGINT!\n", 14);
  Cleanup();
  exit(0);
}

// Helper receiver.
void receive_message(int client_fd) {
  char buffer[BUFFER_SIZE];
  int len = read(client_fd, buffer, sizeof(buffer) - 1);
  buffer[len] = '\0';
}

// Helper to print filerow entirely.
int WriteResultFromFile(char *file, long row_id, int client_fd) {
  FILE *cfPtr;

  if ((cfPtr = fopen(file, "r")) == NULL) {
    printf("File could not be opened\n");
    return -1;
  } else {
    char row[BUFFER_SIZE];

    int i = 0;

    while (i < row_id) {
      char* res = fgets(row, BUFFER_SIZE, cfPtr);
      if (res == NULL) break;
      i++;
    }
    char* res = fgets(row, BUFFER_SIZE, cfPtr);
    write(client_fd, res, strlen(res));  // Write the file row entirely.
    receive_message(client_fd);  // Should be an Ack
    fclose(cfPtr);
    return 0;
  }
}

// Helper to run the query.
void RunQuery(char *term, int client_fd) {
  SearchResultIter results1 = FindMovies(docIndex, term);

  if (results1 == NULL) {
    printf("No results for this term. Please try another.\n");
    int default_num = 0;
    printf("SERVER SENDING: %d\n", default_num);
    write(client_fd, &default_num, sizeof(default_num));
    receive_message(client_fd);  // Should be an ack
    return;  // Should return early.

  } else {
    SearchResult sr = (SearchResult)malloc(sizeof(*sr));
    if (sr == NULL) {
      printf("Couldn't malloc SearchResult in main.c\n");
      return;
    }
    int result;
    char *filename;
    // Get the last
    SearchResultGet(results1, sr);
    filename = GetFileFromId(docs, sr->doc_id);

    int counter = 1;  // Initialization ensures the results are not null.

    while (SearchResultIterHasMore(results1) != 0) {
      counter++;
      SearchResultNext(results1);
    }

    printf("SERVER SENDING: %d\n", counter);
    write(client_fd, &counter, sizeof(counter));
    receive_message(client_fd);  // Should be an ack

    free(sr);
    DestroySearchResultIter(results1);

    // Portion handling the writing.
    SearchResultIter results2 = FindMovies(docIndex, term);
    SearchResult sr2 = (SearchResult)malloc(sizeof(*sr));
    SearchResultGet(results2, sr2);

    WriteResultFromFile(filename, sr2->row_id, client_fd);

    // Check if there are more
    while (SearchResultIterHasMore(results2) != 0) {
      result =  SearchResultNext(results2);
      if (result < 0) {
        printf("error retrieving result\n");
        break;
      }
      SearchResultGet(results2, sr2);
      char *filename = GetFileFromId(docs, sr2->doc_id);
      WriteResultFromFile(filename, sr2->row_id, client_fd);
    }

    free(sr2);
    DestroySearchResultIter(results2);
  }
}

/**
 * Return 0 for successful connection;
 * Return -1 for some error.
 */
int HandleClient(int client_fd, char* query) {
  RunQuery(query, client_fd);
  SendGoodbye(client_fd);
  close(client_fd);
  return 0;
}

/*
 * Taken from: csapp.c - Functions for the CS:APP3e book
 */
void unix_error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

// Wrapper class to ensure proper fork handling.
pid_t Fork(void) {
    pid_t pid;

    if ((pid = fork()) < 0)
      unix_error("Fork error");
    return pid;
}


/**
 *
 */
int HandleConnections(int sock_fd, int debug) {
  printf("Waiting for connection...\n");
  int client_fd = accept(sock_fd, NULL, NULL);  // Need to check.
  printf("Connection made: client_fd=%d\n", client_fd);
  SendAck(client_fd);

  char query[BUFFER_SIZE];
  int len = read(client_fd, query, sizeof(query) - 1);
  query[len] = '\0';
  printf("SERVER RECEIVED: %s \n", query);

  // Check if response is goodbye.
  if (CheckGoodbye(query) == 0) {
    close(client_fd);
    HandleConnections(sock_fd, debug);
  } else {  // This indicates it was a query.
    pid_t pid;
    pid = Fork();
    if (pid == 0) {  // Child process
      if (debug != 0) {
        sleep(10);
      }
      HandleClient(client_fd, query);
    }
  }

  HandleConnections(sock_fd, debug);

  return 0;
}

int Setup(char *dir) {
  struct sigaction sa;

  sa.sa_handler = sigchld_handler;  // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  struct sigaction kill;

  kill.sa_handler = sigint_handler;
  kill.sa_flags = 0;  // or SA_RESTART
  sigemptyset(&kill.sa_mask);

  if (sigaction(SIGINT, &kill, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("Crawling directory tree starting at: %s\n", dir);
  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(dir, docs);
  printf("Crawled %d files.\n", NumElemsInHashtable(docs));

  // Create the index
  docIndex = CreateMovieTitleIndex();

  if (NumDocsInMap(docs) < 1) {
    printf("No documents found.\n");
    return 0;
  }

  // Index the files
  printf("Parsing and indexing files...\n");
  ParseTheFiles(docs, docIndex);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
  return NumElemsInHashtable(docIndex->ht);
}

int Cleanup() {
  DestroyMovieTitleIndex(docIndex);
  DestroyDocIdMap(docs);
  return 0;
}

int main(int argc, char **argv) {
  // port
  char *port = NULL;
  char *dir_to_crawl = NULL;

  int debug_flag = 0;
  int c;

  while ((c = getopt(argc, argv, "dp:f:")) != -1) {
    switch (c) {
      case 'd':
        debug_flag = 1;
        break;
      case 'p':
        port = optarg;
        break;
      case 'f':
        dir_to_crawl = optarg;
        break;
      }
  }

  if (port == NULL) {
    printf("No port provided; please include with a -p flag.\n");
    exit(0);
  }

  if (dir_to_crawl == NULL) {
    printf("No directory provided; please include with a -f flag.\n");
    exit(0);
  }

  int num_entries = Setup(dir_to_crawl);
  if (num_entries == 0) {
    printf("No entries in index. Quitting. \n");
    exit(0);
  }

  int s;

  struct addrinfo hints, *result;

  // Allows "global"
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; /* IPv4 only */
  hints.ai_socktype = SOCK_STREAM; /* TCP */
  hints.ai_flags = AI_PASSIVE;

  s = getaddrinfo("127.0.0.1", port, &hints, &result);

  // If I can't get the address, write an error.
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }

  // Creating the socket.
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Bind the socket
  if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
    perror("bind()");
    exit(1);
  }

  // Listen socket
  if (listen(sock_fd, 10) != 0) {
    perror("listen()");
    exit(1);
  }

  struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
  printf("Listening on fd:%d, p:%d\n", sock_fd, ntohs(result_addr->sin_port));
  freeaddrinfo(result);  // No longer need this.

  // Run the client portion.
  HandleConnections(sock_fd, debug_flag);

  // Got Kill signal
  close(sock_fd);
  Cleanup();
  return 0;
}
