// listener.c -- A simple UDP datagram socket server
#include <stdio.h>      // Standard I/O functions (printf, fprintf)
#include <stdlib.h>     // Memory management and exit()
#include <unistd.h>     // POSIX socket functions (close(), recvfrom())
#include <errno.h>      // Error number definitions (perror())
#include <string.h>     // String manipulation (memset)
#include <sys/types.h>  // Basic types like socklen_t
#include <sys/socket.h> // Socket creation and operations
#include <netinet/in.h> // Internet protocol header for IPv4/IPv6
#include <arpa/inet.h>  // IP address conversion functions
#include <netdb.h>      // Network database functions (getaddrinfo)

// Constants at the top of the file
#define MYPORT "4950"    // Port number to listen on
#define MAXBUFLEN 100    // Maximum buffer length for received data

// Helper function to get the IP address from a sockaddr struct
void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {  // Check for IPv4 address
        return &(((struct sockaddr_in*)sa)->sin_addr);  // Return pointer to IPv4 address
    } else {                        // For IPv6
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

int main(void) {
    int sockfd;                  // Socket file descriptor
    struct addrinfo hints, *servinfo, *p;  // Address information structures
    int rv;                      // Return value from getaddrinfo()
    int numbytes;                // Number of bytes received
    struct sockaddr_storage their_addr;  // Client address (supports IPv4/IPv6)
    char buf[MAXBUFLEN];         // Buffer to store incoming data
    socklen_t addr_len = sizeof their_addr;  // Length of client's address
    char s[INET6_ADDRSTRLEN];     // Temporary string for IP address conversion

    // Initialize the hints structure with zero values
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // Prefer IPv6 (can be changed to AF_INET for IPv4)
    hints.ai_socktype = SOCK_DGRAM;   // UDP socket type
    hints.ai_flags = AI_PASSIVE;      // Use local machine's IP address

    // Resolve the port number and get a list of possible addresses
    if((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddr: %s\n", gai_strerror(rv));
        return 1;  // Exit with error code if resolution fails
    }

    // Try each address in the list until one succeeds
    for(p = servinfo; p != NULL; p = p->ai_next) {
        // Create a new socket using the current address information
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        // Bind the socket to the specified address and port
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);  // Close the failed socket
            perror("listener: bind");
            continue;
        }
        break;  // Exit loop once a valid socket is bound
    }

    // Check if we successfully bound to an address
    if(p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);  // Free memory used by getaddrinfo()

    printf("listener: waiting to receive data...\n");

    // Receive a datagram from the client
    if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,
                          (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    // Print information about the received packet
    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family,
                     get_in_addr((struct sockaddr *)&their_addr),
                     s, sizeof s));

    printf("listener: packet is %d bytes long\n", numbytes);

    // Null-terminate the buffer for string printing
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);  // Close the socket when done
    return 0;       // Exit successfully
}
