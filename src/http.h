#ifndef HTTP_H_
#define HTTP_H_

#include <unistd.h>

enum Http_method {
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
};

// if message_size == -1, sends the status code as body. If message_size == -2, do not send Content-Length header
void send_http_header(int fd, unsigned int http_status, ssize_t message_size);

#endif // HTTP_H_
