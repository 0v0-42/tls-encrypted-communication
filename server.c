#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include "s_ssl.h"
#include "s_thread.h"
//start an listen socket
int setup_socket(int portno) {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR on binding");

    listen(sockfd, 5);
    return sockfd;
}

int connect_to_client(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen) {
    int newsockfd;
    *clilen = sizeof(*cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)cli_addr, clilen);
    if (newsockfd < 0)
        perror("ERROR on accept");
    return newsockfd;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    SSL_CTX *ctx;
    SSL *ssl;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    init_openssl();
    ctx = create_context();
    //version check
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        SSL_CTX_free(ctx);
        ERR_print_errors_fp(stderr);
        perror("Failed to set the minimum TLS protocol version");
    }

    configure_context(ctx);
    portno = atoi(argv[1]);
    sockfd = setup_socket(portno);
    printf("server is running\n");

    while (1) {
        //bulid a security connection
        newsockfd = connect_to_client(sockfd, &cli_addr, &clilen);
        ssl = initialize_ssl_connection(ctx, newsockfd);

        if (ssl) {
            pthread_t send_thread, receive_thread;
            create_and_start_threads(ssl, &send_thread, &receive_thread);
            cleanup_ssl_connection(ssl, newsockfd);
        }
    }

    SSL_CTX_free(ctx);
    close(sockfd);
    printf("server close");
    return 0;
}