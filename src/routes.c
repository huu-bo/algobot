#include <stdlib.h>
#include <string.h>

#include "routes.h"

struct Route routes[] = {
	{
		.type = ROUTE_FILE,
		.url = "/",
		.file = "pages/index.html"
	}
};

struct Route *get_url_route(const char *url) {
	for (size_t i = 0; i < sizeof(routes) / sizeof(struct Route); i++) {
		if (strcmp(routes[i].url, url) == 0) {
			return &routes[i];
		}
	}
	return NULL;
}

