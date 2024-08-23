#define _POSIX_C_SOURCE 200809L

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8080

#define URL_MAX_SIZE 128 // includes null terminator
#define BUFFER_SIZE  1024

enum Http_method {
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
};

// TODO: request routing
// TODO: algo on browser + bot in server

#include "status.h"
#include "routes.h"

// if message_size == -1, sends the status code as body. If message_size == -2, do not send Content-Length header
void send_http_header(int fd, unsigned int http_status, ssize_t message_size) {
	const char *status_msg = http_status_string(http_status);
	if (status_msg == NULL) {
		status_msg = "(null)";
	}
	dprintf(fd, "HTTP/1.1 %d %s\r\n", http_status, status_msg);

	if (message_size != -2) {
		unsigned int content_length;
		if (message_size == -1) {
			content_length = strlen(status_msg) + 4;
		} else {
			content_length = message_size;
		}
		dprintf(fd, "Content-Length: %u\r\n", content_length);
	}
	dprintf(fd, "\r\n");

	if (message_size == -1) {
		dprintf(fd, "%u %s", http_status, status_msg);
	}

	printf("status code %d %s\n", http_status, status_msg);
}

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("error creating socket");
		return EXIT_FAILURE;
	}

	{
		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			perror("error setting socket to reuseaddr and reuseport");
		}
	}

	{
		struct sockaddr_in address = {
			.sin_family = AF_INET,
			.sin_addr = {INADDR_ANY},
			.sin_port = htons(PORT)
		};

		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
			perror("error binding socket");
			return EXIT_FAILURE;
		}
	}

	if (listen(server_fd, 0) == -1) {
		perror("error listening on socket");
		return EXIT_FAILURE;
	}

	while (1) {
		int client_fd;
		if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
			perror("error accepting socket");
			continue;
		}

#define HTTP_EOF_ERROR \
	fprintf(stderr, "unexpected eof in http request header\n"); \
	send_http_header(client_fd, 400, 0); \
	close(client_fd); \
	continue;

		char method_buffer[5] = {0};
		if (read(client_fd, method_buffer, 4) != 4) {
			HTTP_EOF_ERROR
		}

		enum Http_method method;
		if (strcasecmp(method_buffer, "post") == 0) {
			char space;
			if (read(client_fd, &space, sizeof(space)) != sizeof(space) || space != ' ') {
				HTTP_EOF_ERROR
			}
			method = HTTP_METHOD_POST;
		} else if (strcasecmp(method_buffer, "get ") == 0) {
			method = HTTP_METHOD_GET;
		} else {
			fprintf(stderr, "unsupported method, first four bytes of method are '%s'\n", method_buffer);
			send_http_header(client_fd, 501, 0);
			close(client_fd);
			continue;
		}

		char url[URL_MAX_SIZE] = {0}; {
			char c = 0;
			size_t index = 0;
			while (1) {
				if (read(client_fd, &c, sizeof(c)) != sizeof(c)) {
					goto url_eof;
				}
				if (c == ' ') {
					break;
				}

				url[index++] = c;
				if (index > URL_MAX_SIZE - 1 /* -1 for null terminator */) {
					goto url_too_large;
				}
			}
			goto url_ok;

url_eof:
			HTTP_EOF_ERROR
url_too_large:
			fprintf(stderr, "URL too large\n");
			send_http_header(client_fd, 414, -1);
			close(client_fd);
			continue;
		}
url_ok:
		switch (method) {
			case (HTTP_METHOD_GET): {
							printf("get ");
						} break;
			case (HTTP_METHOD_POST): {
							 printf("post ");
						 }
		}
		printf("%s\n", url);

		struct Route *route = get_url_route(url);
		if (route == NULL) {
			send_http_header(client_fd, 404, -1);
			close(client_fd);
			continue;
		}

		if (route->type == ROUTE_FILE) {
			FILE *page_file = fopen(route->file, "r");
			if (page_file == NULL) {
				fprintf(stderr, "error opening file '%s' for serving: %s\n", route->file, strerror(errno));
				send_http_header(client_fd, 500, -1);
				close(client_fd);
				continue;
			}

			fseek(page_file, 0, SEEK_END);
			size_t file_size = ftell(page_file);
			fseek(page_file, 0, SEEK_SET);

			send_http_header(client_fd, 200, file_size);

			char *buffer[BUFFER_SIZE];
			size_t length;
			while ((length = fread(buffer, 1, BUFFER_SIZE, page_file)) != 0) {
				if (write(client_fd, buffer, length) == -1) {
					perror("error while serving file");
				}
			}

			close(client_fd);
			continue;
		} else if (route->type == ROUTE_FUNC) {
			route->func(client_fd);

			close(client_fd);
			continue;
		}

		send_http_header(client_fd, 500, -1);
		close(client_fd);
	}
}
