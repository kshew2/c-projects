// client.c stream socket for client demo

#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // exit
#include <unistd.h>     // close
#include <errno.h>      // errno (error reporting)
#include <string.h>     // memset
#include <netdb.h>      // getaddrinfo, addrinfo struct
#include <sys/types.h>
#include <netinet/in.h> // sockaddr_in (IPv4)
#include <sys/socket.h> // socket functions
#include <arpa/inet.h>  // inet_ntop (convert IP to string)

#define PORT "3490"           // Port the client will connect to (string for getaddrinfo)
#define MAXDATASIZE 100       // Max number of bytes we can receive at once

// Helper function: extract IP address from sockaddr (works for IPv4 or IPv6)
void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET) {
        // IPv4: cast to sockaddr_in and return pointer to sin_addr
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    // IPv6: cast to sockaddr_in6 and return pointer to sin6_addr
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
    int sockfd, numbytes;              // sockfd = socket descriptor, numbytes = bytes received
    char buf[MAXDATASIZE];             // buffer to store incoming data
    struct addrinfo hints, *servinfo, *p;
    // hints: criteria for getaddrinfo
    // servinfo: linked list of results
    // p: iterator through list
    int rv;                            // return value for error checking
    char s[INET6_ADDRSTRLEN];          // buffer to store printable IP string
    
    // Ensure correct number of arguments
    if(argc !=2){
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }
    
    // Zero out hints structure (important to avoid garbage values)
    memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_UNSPEC;       // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP (stream socket)
    
    // Resolve hostname (argv[1]) and port into a list of addresses
    if((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // Loop through all results and try to connect to the first one that works
    for(p = servinfo; p != NULL; p = p->ai_next){   // FIXED: must assign p = p->ai_next
        // Create a socket using the parameters from this addrinfo
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("client: socket");
            continue; // try next address
        }
        
        // Convert the binary IP address into a readable string
        inet_ntop(p->ai_family,
                  get_in_addr((struct sockaddr *)p->ai_addr),
                  s, sizeof s);
        printf("client: attempting connection to %s\n", s);
        
        // Attempt to connect to the server
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("client: connect");
            close(sockfd);  // close failed socket
            continue;       // try next address
        }
        
        break; // success! stop looping
    }
    
    // If we never connected successfully
    if(p==NULL){
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    // Print the address we successfully connected to
    inet_ntop(p->ai_family,
              get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connected to %s\n", s);
    
    // Free the linked list returned by getaddrinfo
    freeaddrinfo(servinfo);
    
    // Receive data from the server
    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    // Null-terminate the received data so it can be printed as a string
    buf[numbytes] = '\0';
    
    // Print the received message
    printf("client: received '%s'\n", buf);
    
    // Close the socket (like closing a file)
    close(sockfd);
    
    return 0;
}
