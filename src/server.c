// src/server.c
#include "server.h"
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

void route_request(gnutls_session_t session, const char *request) {
    char response[1024];
    handle_get_request(request, response);
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

    // Create a TCP socket
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

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return;
    }

    // Initialize GnuTLS
    if (gnutls_certificate_allocate_credentials(&x509_cred) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Failed to allocate credentials\n");
        close(server_fd);
        return;
    }

    // Load the server certificate and private key
    if (gnutls_certificate_set_x509_key_file(x509_cred, cert_path, key_path, GNUTLS_X509_FMT_PEM) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Failed to load certificate or key\n");
        gnutls_certificate_free_credentials(x509_cred);
        close(server_fd);
        return;
    }

    // Create the GnuTLS session
    gnutls_init(&session, GNUTLS_SERVER);
    gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, x509_cred);
    gnutls_priority_set_direct(session, "NORMAL", NULL);

    printf("Server running on port 4433\n");

    while (1) {
        // Accept incoming client connections
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Associate the GnuTLS session with the client socket
        gnutls_transport_set_int(session, client_fd);

        // Perform the TLS handshake
        int ret = gnutls_handshake(session);
        if (ret < 0) {
            fprintf(stderr, "GnuTLS handshake failed: %s\n", gnutls_strerror(ret));
            close(client_fd);
            continue;
        }

        // Receive and process the client's request
        char buffer[1024] = {0};
        ssize_t recv_len = gnutls_record_recv(session, buffer, sizeof(buffer));
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            route_request(session, buffer);
        } else {
            fprintf(stderr, "Failed to receive data: %s\n", gnutls_strerror(recv_len));
        }

        // Terminate the TLS session
        gnutls_bye(session, GNUTLS_SHUT_WR);
        close(client_fd);
    }

    // Clean up GnuTLS resources
    gnutls_deinit(session);
    gnutls_certificate_free_credentials(x509_cred);
    close(server_fd);
}
