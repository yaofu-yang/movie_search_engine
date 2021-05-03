- External Resources
  - https://www.geeksforgeeks.org/socket-programming-cc/
  - https://stackoverflow.com/questions/24766627/connect-to-port-using-tcp-with-c
  - https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/netdb.h/getaddrinfo

Building code:
 - Run 'make all' to create the binaries queryserver, queryclient, multiserver

Running code:
 1a) Start the single-threaded server.
       --> Command: './queryserver -f directory_name/ -p 1500'
       --> Directories available: 'data_tiny' && 'data_small'
 1b) Alternatively, start the multi-threaded server.
       --> Command: './multiserver -f directory_name/ -p 1500'
       --> Same directories as above.     
       
 b) In another window, connect to the server using the client.
       --> Command: './queryclient localhost 1500'
       --> The client side will prompt for search queries.
       
Output:
 - Successful queries will generate a list of movie titles containing the search
 term and print this to the console.
 - Unsuccessful queries will print a message indicating there were no matches and
 continue to prompt for queries.
 - 'q' or 'quit' will exit the client side.
       
 
 