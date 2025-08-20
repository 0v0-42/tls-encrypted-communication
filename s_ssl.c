#include "s_ssl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Initialize OpenSSL library.
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}
// Create an SSL context.
SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}
// Configure the SSL context.
void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
    // Load the server's certificate
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // Load the private key
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}
// Initialize an SSL connection.
SSL *initialize_ssl_connection(SSL_CTX *ctx, int newsockfd) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, newsockfd);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(newsockfd);
        return NULL;
    }
    return ssl;
}
// Cleanup SSL resources.
void cleanup_ssl_connection(SSL *ssl, int newsockfd) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(newsockfd);
}
