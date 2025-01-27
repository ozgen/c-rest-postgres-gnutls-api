//
// Created by moezgen on 1/17/25.
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

typedef struct {
    int status_code;
    char body[1024];
} HttpResponse;

#endif //HTTP_REQUEST_H
