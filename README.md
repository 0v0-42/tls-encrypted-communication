# describe
This is a simple program used to implement one client and one server two-way multiple tls encrypted communication. Server side and client side can both send and recive message, no need to wait the response.

## project structure
```
├── Makefile
├── README.md
├── c_ssl.c
├── c_ssl.h
├── c_thread.c
├── c_thread.h
├── client
├── client.c
├── client.crt
├── client.key
├── s_ssl.c
├── s_ssl.h
├── s_thread.c
├── s_thread.h
├── server
├── server.c
├── server.crt
└── server.key
```
client.c:main function of clint

c_ssl.c: the function about client ssl

c_thread.c: the function about client send and receive message

server.c:main function of server

s_ssl.c: the function about server ssl
s_thread.c: the function about server send and receive message

clinet and server:compiled file

client.crt:public key of client

server.crt:public key of server

client.key: private key of client

server.key:private key of server

## dependency
OpenSSL 3.0.13 30 Jan 2024 (Library: OpenSSL 3.0.13 30 Jan 2024)
```
    sudo apt install openssl
    sudo apt-get install libssl-dev
```
## how to run this project
1. open an terminal, make sure all dependency are downlodad
2. input "make client" and "make server"
3. input "./server 4040"(or other port you choose),you will see "server is running"
4. open another terminal, input "./client 127.0.0.1 4040"(same port with server), you will see"Please enter the message:"
5. input an message to server, and click enter to send message
6. you will see your message shows on the server running terminal. you can see a client message and sending time, such as:"Client: [2024-10-10 15:32:22] a?" and "Please enter the message:" on server side
7. you can keep input message on client side or server side now.
8.  press crtl+C to end the program in both side.