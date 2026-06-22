#include <stdlib.h>
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


//TODO: Implement an http server. Start small with the elements
//add Socket creation, port binding, listening... socket(), bind(), listen()
//accepting connections: accept()
//reading/writing data: read(), write()
//handle http request; parse paths and send responses

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

{
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != ) {
		fprintf(stderr, "gai error: %s\n", gai_strerror(status));
		exit(1);
	}
	
	//
	
	freeaddrinfo(servinfo);//free linked lits
}

struct addrinfo{
	int ai_flags;
	int ai_family; 
	int ai_socktype;
	int ai_protocol;
	size_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_cannonname;
	
	struct addrinfo *ai_next;
}

struct sockaddr_in{
	
	short int sin_family;
	unsigned short int sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
}

struct in_addr{
	uint32_t s_addr;
}

struct sockaddr_in6{
	u_int16_t sin6_family;
	u_int16_t sin6_port;
	u_int32_t sin6_flowinfo;
	struct in6_addr sin6_addr;
	u_int32_t sin6_scope_id;
}

struct in6_addr{
	unsigned char s6_addr[16];
}

struct sockaddr_storage{
	sa_family ss_family;
	char __ss_pad1[_SS_PAD1SIZE];
	int64_t __ss_align;
	char __ss_pad2[_SS_PAD2SIZE];
}

{
	int s;
	struct addrinfo hints, *res;
	
	//do the lookup
	//[pretend we already filled out the "hints" struct]
	getaddrinfo("www.example.com", "http", &hints, &res);
	//you should to error checking on getaddrinfo()
	//the res linked list looking for valid entries instead of just assuming
	//the first is good.
	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
}

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
//binding socket to host program is on eg: 3490
{
	struct addrinfo hints, *res;
	int sockfd;
	
	//first, load up address struct with getaddrinfo();
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL, "3490", &hints, &res);
	
	//make socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	//bind it to the port we passed in to getaddrinfo()
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	
	}
	
	//old version of packing sockaddr_in
	{
		int sockfd;
		struct sockaddr_in my_addr;
		
		sockfd=socket(PF_INET, SOCK_STREAM, 0);
		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(MYPORT);
		my_addr.sin_addr.s_addr = inet_addr("10.12.110.57");
		bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr);
	}
	
	int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
	
	// example where we make a socket connection to example on port 3490
	{
		struct addrinfo hints, *res;
		int sockfd;
		//first, load up address struct with getaddrinfo()
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		getaddrinfo("www.example.com", "3490", &hints, &res);
		//make a socket: 
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		//connect
		connect(sockfd, res->ai_addr, res->ai_addrlen);
	}
	int listen(int sockfd, int backlog);
	//when listening order goes as follows
	{
			getaddrinfo();
			socket();
			bind();
			listen();
			//accept goes here
	}
	
	int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	
	//sample for bind, listen, accept
	{
		#define MYPORT "3490"
		#define BACKLOG 10
		
		int main(void){
			struct sockaddr_storage their_addr;
			socklen_t addr_size;
			struct addrinfo hints, *res;
			int sockfd, new_fd;
			
			//dont forget to error check calls
			//load up address structs with get addrinfo()
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;
			
			getaddrinfo(NULL, MYPORT, &hints, &res);
			
			//make a socket, bind it, and listen
			sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			
			bind(sockfd, res->ai_addr, res->ai_addrlen);
			listen(sockfd, BACKLOG);
			
			//now accept an incoming connection
			addr_size = sizeof their_addr;
			new_fd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);
		}
	}
	
	int send(int sockfd, const void *msg, int len, int flags);
	
	char *msg = "Beej was here";
	int len, bytes_sent;
	
	len = strlen(msg);
	bytes_sent = send(sockfd, msg, len, 0);
	
	int recv(int sockfd, void *buf, int len, int flags);
	
	
	int sendto(int sockfd, const void *msg, int len, unsigned int flags, 
				const struct sockaddr *to, socklen_t tolen);
	
	int recvfrom(int sockfd, void *buf, int len, unsigned int flags, 
				struct sockaddr *from, int *fromlen);

int main(){
	
}



