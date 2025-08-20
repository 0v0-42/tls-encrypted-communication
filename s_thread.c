#include "s_thread.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <openssl/err.h>

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int server_running = 1;
// Create and start send and receive threads
void create_and_start_threads(SSL *ssl, pthread_t *send_thread, pthread_t *receive_thread) {
    if (pthread_create(receive_thread, NULL, receive_message, ssl) != 0) {
        perror("Failed to create receive thread");
        pthread_cancel(*send_thread);
        exit(EXIT_FAILURE);
    }
    if (pthread_create(send_thread, NULL, send_message, ssl) != 0) {
        perror("Failed to create send thread");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(*send_thread, NULL) != 0) {
        perror("Failed to join send thread");
    }
    if (pthread_cancel(*receive_thread) != 0) {
        perror("Failed to cancel receive thread");
    }
}
// Get the current time as a string
void get_current_time(char *time_str, size_t size) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_str, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}
// Thread function: receive message
void *receive_message(void *arg) {
    SSL *ssl = (SSL *)arg;
    char receive_buffer[256];
    char message_body[256];
    int n;

    while (server_running) {
        bzero(receive_buffer, sizeof(receive_buffer));
        n = SSL_read(ssl, receive_buffer, sizeof(receive_buffer) - 1);
        if (n > 0) {
            snprintf(message_body, sizeof(message_body), "%.*s",  n, receive_buffer);
            //get the message without \n
            pthread_mutex_lock(&print_lock);
            printf("\nClient: %s\n", message_body);
            printf("Please enter the message: ");
            fflush(stdout);
            pthread_mutex_unlock(&print_lock);
        } else if (n == 0) {
            printf("Connection closed by client\n");
            break;
        } else {
            ERR_print_errors_fp(stderr);
            break;
        }
    }

    return NULL;
}
// Thread function: send message
void *send_message(void *arg) {
    SSL *ssl = (SSL *)arg;
    char send_buffer[256];
    int n;
    char time_added_buffer[64];
    char message[350];

    while (server_running) {
        get_current_time(time_added_buffer, sizeof(time_added_buffer));
        pthread_mutex_lock(&print_lock);
        printf("Please enter the message: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

        bzero(send_buffer, sizeof(send_buffer));
        if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL) {
            break;
        }        

        snprintf(message, sizeof(message), "[%s] %.*s", time_added_buffer, (int)strcspn(send_buffer, "\n"), send_buffer);//add a timestamp
        n = SSL_write(ssl, message, strlen(message));
        if (n <= 0) {
            ERR_print_errors_fp(stderr);
            break;
        }
    }

    return NULL;
}
