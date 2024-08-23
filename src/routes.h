#ifndef ROUTES_H_
#define ROUTES_H_

struct Route {
	enum {
		ROUTE_FILE,
		ROUTE_FUNC
	} type;
	const char *url;

	union {
		const char *file;
		void (*func)();
	};
} extern routes[];

// Returns NULL if url is not in routes
struct Route *get_url_route(const char *url);

#endif // ROUTES_H_
