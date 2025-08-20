#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>
#include "c_ssl.h"
#include "c_thread.h"  

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

// connect to server
int connect_to_server(const char *hostname, int portno) {
    int sockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");

    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    init_openssl();
    SSL_CTX *ctx = create_context();

    if (!set_min_tls_version(ctx)) {
        perror("Failed to set the minimum TLS protocol version");
    }
    configure_context(ctx);

    int sockfd = connect_to_server(argv[1], atoi(argv[2]));
    SSL *ssl = initialize_ssl_connection(ctx, sockfd);

    pthread_t send_thread, receive_thread;
    create_and_start_threads(ssl, &send_thread, &receive_thread);

    if (pthread_join(send_thread, NULL) != 0) {
        perror("Failed to join send thread");
    }
    if (pthread_cancel(receive_thread) != 0) {
        perror("Failed to cancel receive thread");
    }

    cleanup(ssl, sockfd, ctx);

    return 0;
}
