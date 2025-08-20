#ifndef S_SSL_H
#define S_SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>

void init_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);
SSL *initialize_ssl_connection(SSL_CTX *ctx, int newsockfd);
void cleanup_ssl_connection(SSL *ssl, int newsockfd);

#endif 