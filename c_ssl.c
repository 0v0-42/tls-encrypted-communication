#include "c_ssl.h"
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
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

// Configure the SSL context.
void configure_context(SSL_CTX *ctx) {
    if (SSL_CTX_load_verify_locations(ctx, "server.crt", NULL) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

// Initialize an SSL connection.
SSL *initialize_ssl_connection(SSL_CTX *ctx, int sockfd) {
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        exit(EXIT_FAILURE);
    }
    return ssl;
}

// Cleanup SSL resources.
void cleanup(SSL *ssl, int sockfd, SSL_CTX *ctx) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
}

// Set the minimum TLS version.
int set_min_tls_version(SSL_CTX *ctx) {
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        SSL_CTX_free(ctx);
        ERR_print_errors_fp(stderr);
        return 0;
    }
    return 1;
}