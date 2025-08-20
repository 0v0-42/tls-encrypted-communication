#ifndef C_SSL_H
#define C_SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>

// Initialize OpenSSL
void init_openssl();

// Create SSL context
SSL_CTX *create_context();

// Configure SSL context
void configure_context(SSL_CTX *ctx);

// Initialize SSL connection
SSL *initialize_ssl_connection(SSL_CTX *ctx, int sockfd);

// Cleanup SSL resources
void cleanup(SSL *ssl, int sockfd, SSL_CTX *ctx);

// Set minimum TLS version
int set_min_tls_version(SSL_CTX *ctx);

#endif