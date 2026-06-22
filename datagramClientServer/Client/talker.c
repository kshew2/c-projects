// talker.c - datagram client demo 
// This program acts as a UDP (User Datagram Protocol) sender, allowing a client 
// to send a message to a specified server hostname and port.

#include <stdio.h>       // Standard Input/Output functions (like printf, fprintf)
#include <stdlib.h>      // General utilities (like exit())
#include <unistd.h>      // Provides access to the POSIX API (like close() and write()/read())
#include <errno.h>       // Error number definitions (e.g., errno)
#include <string.h>      // String manipulation functions (like memset, strlen)

// Networking specific headers:
#include <sys/types.h>   // System data types (for struct definitions)
#include <sys/socket.h>  // Core socket functionality (socket(), sendto())
#include <netinet/in.h>  // Internet protocols structures (like sockaddr_in)
#include <arpa/inet.h>   // Functions for IP address manipulation (like inet_ntoa, getaddrinfo)
#include <netdb.h>       // Network database functions (getaddrinfo())

// Define the default port number we are connecting to
#define SERVERPORT "4950" 

/**
 * Main function: The program entry point.
 * argc: Argument Count (number of arguments passed)
 * argv: Argument Vector (array of strings containing the actual arguments)
 */
int main(int argc, char *argv[]){
    // Declare necessary variables for networking and file handling
    int sockfd;               // File descriptor for the socket
    struct addrinfo hints, *servinfo, *p; // Variables used by getaddrinfo()
    int rv;                    // Return value from functions (used to check for errors)
    int numbytes;             // Will store the number of bytes sent successfully

    /* ------------------- START: Argument Validation -------------------- */
    // Check if exactly three arguments were passed: 
    // 1. ./talker (The program name)
    // 2. hostname (IP or domain, e.g., localhost)
    // 3. message (The text to send)
    if(argc != 3) {
        fprintf(stderr, "usage: talker hostname message\n"); // Print usage error if arguments are wrong
        exit(1);                                          // Exit the program with an error code (1)
    }
    /* ------------------- END: Argument Validation -------------------- */

    // Initialize the 'hints' structure to all zeros. This is crucial for memory safety.
    memset(&hints, 0, sizeof hints); 
    
    // Configure hints struct: We are using IPv6 addresses for connection
    hints.ai_family = AF_INET6; // Sets the address family to IPv6 (Note: You might want AF_UNSPEC or AF_INET)
    hints.ai_socktype = SOCK_DGRAM; // Specifies that we are using a Datagram socket type (UDP).

    /* ------------------- Step 1: Get Address Information -------------------- */
    // Call getaddrinfo to look up the address details for the provided hostname (argv[1]) 
    // and port (SERVERPORT). It fills 'servinfo' with an array of possible addresses.
    rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo);
    if(rv != 0) {
        // If getaddrinfo returns a non-zero value, an error occurred.
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // Print the specific name resolution error
        return 1; // Exit with error code
    }

    /* ------------------- Step 2: Create Socket and Iterate Addresses -------------------- */
    // Loop through every potential address found in 'servinfo' until p becomes NULL.
    for(p = servinfo; p != NULL; p = p->ai_next){
        // Try to create a socket using the current address info (p).
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("talker: socket"); // If socket creation fails, print the system error message.
            continue;                // Skip this failed address and try the next one.
        }
        break; // Success! We successfully created a socket on this address, so we break out of the loop.
    }

    if(p == NULL){
        fprintf(stderr, "talker: failed to create socket\n"); // If the loop completed without success
        return 2;
    }

    /* ------------------- Step 3: Send Data (The main action) -------------------- */
    // Use sendto() to package and send the data.
    // Arguments:
    // - sockfd: The socket descriptor we just created.
    // - argv[2]: The message content (the text provided by the user).
    // - strlen(argv[2]): The length of the message in bytes.
    // - 0: Flags (usually zero).
    // - p->ai_addr: The destination address structure.
    // - p->ai_addrlen: The size of that destination address structure.
    if((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1){
        perror("talker: sendto"); // If sending fails, print the system error message.
        exit(1);                  // Exit program on failure.
    }

    /* ------------------- Step 4: Cleanup and Success Message -------------------- */
    freeaddrinfo(servinfo); // Release memory allocated by getaddrinfo()
    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]); // Report success
    close(sockfd);         // Close the socket descriptor (release system resource)
    return 0;               // Exit successfully
}
