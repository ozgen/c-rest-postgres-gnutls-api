#include "data_handler.h"
#include "data_service.h"
#include <stdio.h>
#include <string.h>

void handle_get_request(const char *request, char *response) {
    if (strstr(request, "GET /data")) {
        const char *data = get_data_service();
        snprintf(response, 1024, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%s", strlen(data), data);
    } else {
        snprintf(response, 1024, "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found");
    }
}