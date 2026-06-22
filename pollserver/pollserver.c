/*
 * Poll-Based Chat Server Implementation
 *
 * This program creates a TCP server that listens on port 9034 and allows multiple clients to connect.
 * It uses the POSIX poll() system call for efficient non-blocking I/O handling.
 */

#include <stdio.h>      // Standard input/output functions (printf, fprintf)
#include <stdlib.h>     // General utilities (malloc, realloc, exit)
#include <string.h>     // String manipulation functions
#include <unistd.h>     // POSIX functions (close, read, write)
#include <sys/types.h>  // Socket types and definitions
#include <sys/socket.h> // Socket creation and communication
#include <netinet/in.h> // Internet socket definitions
#include <arpa/inet.h>  // IP address manipulation
#include <netdb.h>      // Host name resolution
#include <poll.h>       // Poll system call for non-blocking I/O

#define PORT "9034"     // Port number the server will listen on

// Helper function to convert socket addresses to human-readable strings
const char *inet_ntop2(void *addr, char *buf, size_t size) {
    /*
     * Converts a binary IP address (either IPv4 or IPv6) into a string representation.
     *
     * Parameters:
     *   addr: Pointer to the socket address structure (struct sockaddr_in or sockaddr_in6)
     *   buf: Buffer to store the converted string
     *   size: Size of the buffer
     */
    struct sockaddr_storage *sas = addr;  // Generic socket address structure
    struct sockaddr_in *sa4;              // IPv4-specific structure
    struct sockaddr_in6 *sa6;             // IPv6-specific structure
    void *src;

    switch(sas->ss_family) {
        case AF_INET:   // IPv4
            sa4 = addr;
            src = &(sa4->sin_addr);
            break;
        case AF_INET6:  // IPv6
            sa6 = addr;
            src = &(sa6->sin6_addr);
            break;
        default:
            return NULL;  // Unsupported address family
    }

    return inet_ntop(sas->ss_family, src, buf, size);  // Convert to string
}

// Creates and configures a listening socket for the server
int get_listener_socket(void) {
    /*
     * Sets up a TCP socket that listens on the specified port.
     *
     * Returns:
     *   -1 if socket creation or binding fails
     *   Valid file descriptor (socket) otherwise
     */
    int listener;  // Socket descriptor for listening
    int yes = 1;   // Value to set SO_REUSEADDR option
    int rv;        // Return value from system calls

    struct addrinfo hints, *ai, *p;  // Address information structure

    /*
     * Get address info for the server's IP and port.
     * We want IPv4 (AF_INET) sockets that are passive listeners (AI_PASSIVE).
     */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;     // Allow binding to any interface

    /*
     * Resolve the hostname (NULL means use localhost) and port.
     * If resolution fails, exit with error message.
     */
    if((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    /*
     * Try to create a socket for each address in the list.
     * Bind it and reuse the port if needed (SO_REUSEADDR).
     */
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(listener < 0) continue;  // Skip if socket creation fails

        /*
         * Allow the socket to reuse the address even if it's in TIME_WAIT state.
         * This prevents "Address already in use" errors on restart.
         */
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if(bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);  // Close socket and try next one
            continue;
        }
        break;  // Successfully bound to an address
    }

    /*
     * If we got here without binding, return -1 (error).
     */
    if(p == NULL) {
        freeaddrinfo(ai);
        return -1;
    }

    freeaddrinfo(ai);  // Free the resolved address info

    /*
     * Start listening for incoming connections with a backlog of 10.
     * This means up to 10 queued connections before new ones are accepted.
     */
    if(listen(listener, 10) == -1) {
        return -1;
    }

    return listener;  // Return the valid socket descriptor
}

// Adds a new file descriptor (socket) to the poll set
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size) {
    /*
     * Dynamically grows the array of file descriptors when needed.
     *
     * Parameters:
     *   pfds: Pointer to pointer to the pollfd array (needs reallocation)
     *   newfd: New socket descriptor to add
     *   fd_count: Current count of active sockets
     *   fd_size: Current capacity of the array
     */
    // Double the size if we're out of space
    if(*fd_count == *fd_size) {
        *fd_size *= 2;  // Exponential growth for efficiency
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    /*
     * Add the new socket to the end of the array.
     * Set POLLIN (ready to read) and clear any existing events/revents.
     */
    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN;  // Watch for incoming data
    (*pfds)[*fd_count].revents = 0;      // No events yet
    (*fd_count)++;  // Increment the count of active sockets
}

// Removes a file descriptor from the poll set (when a client disconnects)
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
    /*
     * Moves the last element in the array to fill the gap left by removal.
     *
     * Parameters:
     *   pfds: Array of pollfd structures
     *   i: Index of the socket to remove
     *   fd_count: Current count of active sockets (will decrement)
     */
    // Shift all elements after index 'i' one position left
    for(int j = i; j < (*fd_count - 1); j++) {
        pfds[j] = pfds[j + 1];
    }
    (*fd_count)--;  // Decrement the count of active sockets
}

// Handles a new incoming connection from a client
void handle_new_connection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds) {
    /*
     * Accepts a new connection and adds it to the poll set.
     *
     * Parameters:
     *   listener: Socket descriptor for listening
     *   fd_count: Current count of active sockets
     *   fd_size: Current capacity of the array
     *   pfds: Pointer to pointer to the pollfd array
     */
    struct sockaddr_storage remoteaddr;  // Client's address information
    socklen_t addrlen = sizeof(remoteaddr);  // Size of client address
    int newfd;  // Newly accepted socket descriptor

    /*
     * Accept a connection from the client.
     * If accept() fails, print error and return.
     */
    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
    if(newfd == -1) {
        perror("accept");
        return;
    }

    /*
     * Add the new socket to our poll set for monitoring.
     * Print connection details to console.
     */
    add_to_pfds(pfds, newfd, fd_count, fd_size);
    char remoteIP[INET6_ADDRSTRLEN];
    printf("pollserver: New connection from %s on socket %d\n",
           inet_ntop2(&remoteaddr, remoteIP), newfd);
}

// Handles data received from a client (or their disconnection)
void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i) {
    /*
     * Processes incoming data from a connected client.
     *
     * Parameters:
     *   listener: Socket descriptor for listening
     *   fd_count: Current count of active sockets
     *   pfds: Array of pollfd structures
     *   pfd_i: Index of the socket in the array
     */
    char buf[256];  // Buffer to store received data
    int nbytes = recv(pfds[*pfd_i].fd, buf, sizeof(buf), 0);  // Read from client
    int sender_fd = pfds[*pfd_i].fd;  // Socket descriptor of the sender

    /*
     * Check if the connection was closed by the client (nbytes == 0)
     * or if there was an error (negative nbytes).
     */
    if(nbytes <= 0) {
        if(nbytes == 0) {  // Connection closed gracefully
            printf("pollserver: Socket %d hung up\n", sender_fd);
        } else {           // Error occurred in recv()
            perror("recv");
        }

        /*
         * Close the socket and remove it from our poll set.
         */
        close(pfds[*pfd_i].fd);  // Clean up resources
        del_from_pfds(pfds, *pfd_i, fd_count);
        (*pfd_i)--;  // Re-examine the slot we just deleted (to avoid out-of-bounds)
    } else {  // Data was received successfully

        /*
         * Print the received message to console.
         */
        printf("pollserver: Received from fd %d: %.*s\n", sender_fd, nbytes, buf);

        /*
         * Broadcast the message to all other connected clients (except the sender).
         */
        for(int j = 0; j < *fd_count; j++) {
            int dest_fd = pfds[j].fd;
            if(dest_fd != listener && dest_fd != sender_fd) {  // Skip listener and ourselves
                if(send(dest_fd, buf, nbytes, 0) == -1) {  // Send to all other clients
                    perror("send");
                }
            }
        }
    }
}

// Processes existing connections based on poll results
void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds) {
    /*
     * Iterates through the active sockets and handles events.
     *
     * Parameters:
     *   listener: Socket descriptor for listening
     *   fd_count: Current count of active sockets
     *   fd_size: Current capacity of the array
     *   pfds: Pointer to pointer to the pollfd array
     */
    // Check each socket in our poll set for events (readable, error, etc.)
    for(int i = 0; i < *fd_count; i++) {
        if((*pfds)[i].revents & (POLLIN | POLLHUP)) {  // Check for readable or closed connection

            /*
             * If this is the listener socket, handle a new incoming connection.
             */
            if((*pfds)[i].fd == listener) {
                handle_new_connection(listener, fd_count, fd_size, pfds);
            } else {  // Otherwise, it's a regular client
                handle_client_data(listener, fd_count, *pfds, &i);  // Pass index by reference to avoid copying
            }
        }
    }
}

// Main function: Sets up and runs the server
int main(void) {
    /*
     * Entry point for the program.
     * Creates a listening socket and enters an infinite loop processing connections.
     */
    int listener;  // Socket descriptor for listening
    int fd_size = 5;  // Initial capacity of the pollfd array (can grow dynamically)
    int fd_count = 0;  // Current count of active sockets

    /*
     * Allocate memory for the pollfd array and initialize it.
     */
    struct pollfd *pfds = malloc(sizeof(*pfds) * fd_size);

    /*
     * Get a listening socket on port 9034.
     * If this fails, exit with error message.
     */
    listener = get_listener_socket();
    if(listener == -1) {
        fprintf(stderr, "Error getting listening socket\n");
        exit(1);
    }

    /*
     * Add the listener to our poll set and start the server.
     * The listener is always at index 0 in our array.
     */
    pfds[0].fd = listener;      // Set the file descriptor
    pfds[0].events = POLLIN;    // Watch for incoming connections
    fd_count = 1;              // Only one socket (the listener) initially

    printf("pollserver: Waiting for connections...\n");

    /*
     * Main server loop:
     *   - Uses poll() to wait for events on any of our sockets.
     *   - If poll() returns -1, exit with error.
     *   - Otherwise, process all active connections.
     */
    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);  // Block indefinitely (-1)

        if(poll_count == -1) {  // Error occurred in poll()
            perror("poll");
            exit(1);
        }

        /*
         * Process all active connections based on poll results.
         */
        process_connections(listener, &fd_count, &fd_size, &pfds);

        // Clean up memory when exiting (though loop is infinite)
    }

    free(pfds);  // Free allocated memory (should never reach here)
}
