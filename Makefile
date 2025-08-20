client:client.c c_ssl.c c_thread.c
	gcc client.c c_ssl.c c_thread.c -o client -lssl -lcrypto -lpthread

server:server.c s_ssl.c s_thread.c
	gcc server.c s_ssl.c s_thread.c -o server -lssl -lcrypto -lpthread

all: client server

