#ifndef C_THREAD_H
#define C_THREAD_H

#include <pthread.h>
#include <openssl/ssl.h>


void create_and_start_threads(SSL *ssl, pthread_t *send_thread, pthread_t *receive_thread);
void *send_message(void *arg);
void *receive_message(void *arg);

#endif 