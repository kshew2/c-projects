CC=gcc
CFLAGS=-Wall -g

SERVER_SRC=Server/server.c
CLIENT_SRC=Client/client.c

all: server client

server:
	$(CC) $(CFLAGS) $(SERVER_SRC) -o server_app

client:
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o client_app

clean:
	rm -f server_app client_app
