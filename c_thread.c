#include "c_thread.h"
#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

extern pthread_mutex_t print_lock;

// Create and start send and receive threads
void create_and_start_threads(SSL *ssl, pthread_t *send_thread, pthread_t *receive_thread) {
    if (pthread_create(send_thread, NULL, send_message, ssl) != 0) {
        perror("Failed to create send thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(receive_thread, NULL, receive_message, ssl) != 0) {
        perror("Failed to create receive thread");
        pthread_cancel(*send_thread);
        exit(EXIT_FAILURE);
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

// Thread function: send message
void *send_message(void *arg) {
    SSL *ssl = (SSL *)arg;
    char send_buffer[256];
    char time_added_buffer[64];
    int n;

    while (1) {
        pthread_mutex_lock(&print_lock);
        printf("Please enter the message: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

        bzero(send_buffer, sizeof(send_buffer));
        if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL) {
            break;
        }

        n = strlen(send_buffer);
        char formatted_buffer[256];
        snprintf(formatted_buffer, sizeof(formatted_buffer), "%.*s", (n > 0 && send_buffer[n - 1] == '\n') ? n - 1 : n, send_buffer);

        get_current_time(time_added_buffer, sizeof(time_added_buffer));
        char message[350];
        snprintf(message, sizeof(message), "[%s] %s", time_added_buffer, formatted_buffer);

        n = SSL_write(ssl, message, strlen(message));
        if (n <= 0) {
            ERR_print_errors_fp(stderr);
            break;
        }
    }
    return NULL;
}

// Thread function: receive message
void *receive_message(void *arg) {
    SSL *ssl = (SSL *)arg;
    char receive_buffer[256];
    char message_body[256];
    int n;

    while (1) {
        bzero(receive_buffer, sizeof(receive_buffer));
        n = SSL_read(ssl, receive_buffer, sizeof(receive_buffer) - 1);
        if (n > 0) {
            snprintf(message_body, sizeof(message_body), "%.*s", (n > 0 && receive_buffer[n - 1] == '\n') ? n - 1 : n, receive_buffer);
            pthread_mutex_lock(&print_lock);
            printf("\nServer reply: %s\n", message_body);
            printf("Please enter the message: ");
            fflush(stdout);
            pthread_mutex_unlock(&print_lock);
        } else {
            ERR_print_errors_fp(stderr);
            break;
        }
    }
    return NULL;
}