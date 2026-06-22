// server.c stream socket server demo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490" //the port users will be connecting to
#define BACKLOG 10 //how many pending connections queue will hold

void sigchld_handler(int s){
	(void)s; //quiet unused variable warning;
	
	//waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	
	errno = saved_errno;
}

//get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
		if(sa->sa_family == AF_INET) {
			return &(((struct sockaddr_in*)sa)->sin_addr);
		}
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	// Variables setup:
	int sockfd, new_fd; // sockfd = Server listening socket descriptor; new_fd = Descriptor for a single connected client.
	struct addrinfo hints, *servinfo, *p; 
    // 'hints' holds desired address parameters (e.g., IPv4 only, streaming).
    // 'servinfo' is the result list from getaddrinfo.
    // 'p' iterates through results in servinfo.
	struct sockaddr_storage their_addr;// General structure to hold *any* connector's address info (IPv4 or v6).
	socklen_t sin_size;               // Size needed for the socket structures.
	struct sigaction sa;              // Structure used for setting up signal handlers (SIGCHLD).
	int yes = 1;                      // Flag for SO_REUSEADDR.
	char s[INET6_ADDRSTRLEN];        // Buffer to store the client's IP address as a string.
	int rv;                           // Return value variable.
	
    // --- Step 1: Initialize Hints (The "What I Want" List) ---
	memset(&hints, 0, sizeof hints); // Set all bytes of 'hints' to zero.
	hints.ai_family = AF_INET;       // We only want IPv4 addresses.
	hints.ai_socktype = SOCK_STREAM;// We are using TCP (stream-based) sockets.
	hints.ai_flags = AI_PASSIVE;    // Means we will bind to all available interfaces (IP address 0.0.0.0).
	
    // --- Step 2: Resolve Network Addresses (The "Where to Find It" List) ---
	if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo %s\n", gai_strerror(rv));
		return 1; // Exit if we can't even figure out the IP for this port.
	}
	
    // --- Step 3: Loop and Bind (Finding the Best Socket) ---
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket"); // If creating the socket fails, print an error and try the next one.
			continue;
		}
		
        // Set SO_REUSEADDR: This allows the server to restart quickly by reusing a port that is still in TIME_WAIT state.
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1); // If we can't set this option, something is fundamentally wrong.
		}
		
        // Bind: Attach the socket descriptor (sockfd) to a specific local address/port combination defined by 'p'.
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) { 
			close(sockfd); // If bind fails, close the failed socket.
			perror("server: bind");
			continue;    // Try binding to the next address in the list (if available).
		}
		break; // Success! We bound successfully, so we break out of the loop.
	}
	
	freeaddrinfo(servinfo); // Clean up the memory allocated by getaddrinfo().
	
	if(p == NULL){
		fprintf(stderr, "server: failed to bind\n");
		exit(1); // If we tried all addresses and couldn't bind anywhere.
	}
	
    // --- Step 4: Listen (Ready for Connections) ---
	if(listen(sockfd, BACKLOG) == -1){
		perror("listen"); // Print error if system can't open the port for accepting connections.
		exit(1);
	}
	
    // --- Step 5: Signal Handling Setup (Reaping Zombies) ---
	sa.sa_handler = sigchld_handler; // Tell the OS to use our custom function when a child dies.
	sigemptyset(&sa.sa_mask);      // Clear any signal masks (we don't want blocked signals).
	sa.sa_flags = SA_RESTART;     // If interrupted, restart system calls.
	if(sigaction(SIGCHLD, &sa, NULL) == -1){ 
		perror("sigaction"); // Setup the handler for the SIGCHLD signal (Child has terminated).
		exit(1);
	}
	
	printf("server: waiting for connections...\n");
    // ... continue to loop

	
	while(1) { // accept loop (Runs forever unless stopped)
		sin_size = sizeof their_addr;
        // accept(): BLOCKS until a client connects. It returns a NEW file descriptor for this specific connection.
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); 
		if(new_fd == -1){
			perror("accept"); // Handle error if accepting fails.
			continue;
		}
		
        // Convert the raw address structure into a human-readable IP string.
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("Server: got connection from %s\n", s);
		
        // --- Process Management using fork() ---
		if(!fork()){ 
            // The code inside this block runs ONLY in the CHILD process (the one handling the request).
            // If it returns 0, we are the child.
            
			close(sockfd); // Child doesn't need the main listening socket descriptor; only the parent needs that.
			if(send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
                
			// Clean up the client connection descriptor and exit immediately after processing the request.
			close(new_fd);
			exit(0); 
		}
        // --- End of Child Block ---
        
		// This code runs ONLY in the PARENT process (the listener).
		close(new_fd); // The parent must close the descriptor it received from accept() because it is done with it.
	}
	
	return 0;
}

