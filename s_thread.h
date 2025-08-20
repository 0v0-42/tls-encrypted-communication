#ifndef S_THREAD_H
#define S_THREAD_H

#include <openssl/ssl.h>
#include <pthread.h>

extern pthread_mutex_t print_lock;
extern volatile int server_running;

void create_and_start_threads(SSL *ssl, pthread_t *send_thread, pthread_t *receive_thread);
void *send_message(void *arg);
void *receive_message(void *arg);
void get_current_time(char *time_str, size_t size);

#endif