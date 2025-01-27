#include "server.h"
#include "camera_handler.h"
#include "data_service.h"
#include "data_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <gnutls/gnutls.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define RESPONSE_SIZE 4096

void route_request(gnutls_session_t session, const char *request) {
    char response[RESPONSE_SIZE] = {0};
    const char *get_route = "/data";
    const char *post_route = "/api/v1/camera";

    // Extract HTTP method and path
    char method[16] = {0}; // Allow room for longer methods like "OPTIONS"
    char path[256] = {0};
    sscanf(request, "%15s %255s", method, path);

    // Route logic
    if (strcmp(path, get_route) == 0) {
        if (strcmp(method, "GET") == 0) {
            const char *data = get_data_service();
            char *response = (char * )malloc(256 + strlen(data));
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%s",
                     strlen(data), data);
        } else {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 18\r\n\r\nMethod Not Allowed");
        }
    } else if (strcmp(path, post_route) == 0) {
        if (strcmp(method, "POST") == 0) {
            const char *body_start = strstr(request, "\r\n\r\n");
            if (body_start) {
                body_start += 4; // Skip past the header-body separator
                handle_camera_post_request(request, body_start, response);
            } else {
                snprintf(response, sizeof(response),
                         "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nBad Request");
            }
        } else {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 18\r\n\r\nMethod Not Allowed");
        }
    } else {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found");
    }

    // Send the response
    gnutls_record_send(session, response, strlen(response));
}

void start_server() {
    const char *cert_path = getenv("SERVER_CERT_PATH");
    const char *key_path = getenv("SERVER_KEY_PATH");

    if (!cert_path || !key_path) {
        fprintf(stderr, "SERVER_CERT_PATH and SERVER_KEY_PATH must be set in dev.env\n");
        return;
    }

    gnutls_certificate_credentials_t x509_cred;
    gnutls_session_t session;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(4433),
        .sin_addr.s_addr = INADDR_ANY,
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return;
    }

    if (gnutls_certificate_allocate_credentials(&x509_cred) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Failed to allocate credentials\n");
        close(server_fd);
        return;
    }

    if (gnutls_certificate_set_x509_key_file(x509_cred, cert_path, key_path, GNUTLS_X509_FMT_PEM) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Failed to load certificate or key\n");
        gnutls_certificate_free_credentials(x509_cred);
        close(server_fd);
        return;
    }

    printf("Server running on port 4433\n");

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        gnutls_init(&session, GNUTLS_SERVER);
        gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, x509_cred);
        gnutls_priority_set_direct(session, "NORMAL", NULL);
        gnutls_transport_set_int(session, client_fd);

        if (gnutls_handshake(session) < 0) {
            fprintf(stderr, "GnuTLS handshake failed\n");
            gnutls_deinit(session);
            close(client_fd);
            continue;
        }

        char buffer[BUFFER_SIZE] = {0};
        ssize_t recv_len = gnutls_record_recv(session, buffer, sizeof(buffer));
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            route_request(session, buffer);
        } else {
            fprintf(stderr, "Failed to receive data: %s\n", gnutls_strerror(recv_len));
        }

        gnutls_bye(session, GNUTLS_SHUT_WR);
        gnutls_deinit(session);
        close(client_fd);
    }

    gnutls_certificate_free_credentials(x509_cred);
    close(server_fd);
}
