#include "fs_http_server/fs_http_server.h"

#include "fs_http_server/bstrlib.h"
#include "fs_http_server/darray.h"
#include "fs_http_server/hashmap.h"
#include "fs_http_server/request.h"
#include "fs_http_server/response.h"
#include "fs_http_server/route_matching.h"

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define HTTP_BUFFER_SIZE 1024

static void log_send(int socket_fd, const void *buffer, size_t buffer_size)
{
	if (send(socket_fd, buffer, buffer_size, 0) == -1) {
		printf("Message sending failed: %d: %s \n", errno,
		       strerror(errno));
	}
}

static void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// App
struct HttpApp {
	Hashmap *routes;
	DArray *request_middlewares;
	DArray *response_middlewares;
	bstring directory;
};

HttpAppHandle fs_http_app_new()
{
	HttpAppHandle app = calloc(1, sizeof(*app));
	check_mem(app);

	app->routes = Hashmap_create(NULL, NULL);
	check_mem(app->routes);
	app->request_middlewares =
		DArray_create(sizeof(RequestMiddlewareHandler), 10);
	check_mem(app->request_middlewares);
	app->response_middlewares =
		DArray_create(sizeof(ResponseMiddlewareHandler), 10);
	check_mem(app->response_middlewares);

	return app;

error:
	if (app) {
		fs_http_app_delete(app);
	}

	return NULL;
}

void fs_http_app_delete(HttpAppHandle app)
{
	if (app->routes) {
		Hashmap_destroy(app->routes);
	}

	if (app->request_middlewares) {
		DArray_destroy(app->request_middlewares);
	}

	if (app->response_middlewares) {
		DArray_destroy(app->response_middlewares);
	}

	if (app->directory) {
		bdestroy(app->directory);
	}

	free(app);
}

void fs_http_app_register_route(HttpAppHandle app, bstring pattern,
				RouteHandler handler)
{
	Hashmap_set(app->routes, pattern, handler);
}

void fs_http_app_register_request_middleware(HttpAppHandle app,
					     RequestMiddlewareHandler handler)
{
	DArray_push(app->request_middlewares, handler);
}

void fs_http_app_register_response_middleware(HttpAppHandle app,
					      RequestMiddlewareHandler handler)
{
	DArray_push(app->response_middlewares, handler);
}

// Server
struct HttpServer {
	HttpAppHandle app;
	bstring address;
	int port;

	int fd, client_addr_len;
	struct sockaddr_in client_addr;
};

HttpServerHandle fs_http_server_new(HttpAppHandle app)
{
	HttpServerHandle server = calloc(1, sizeof(*server));
	check_mem(server);

	server->app = app;

	return server;

error:
	if (server) {
		fs_http_server_delete(server);
	}

	return NULL;
}

void fs_http_server_delete(HttpServerHandle server)
{
	server->app = NULL;
	if (server->address) {
		bdestroy(server->address);
	}
	free(server);
}

int fs_http_server_bind(HttpServerHandle server, bstring address, int port)
{
	server->address = address;
	server->port = port;

	server->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server->fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
		       sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = { htonl(INADDR_ANY) },
	};

	int s = inet_pton(AF_INET, bdata(address), &(serv_addr.sin_addr));

	if (bind(server->fd, (struct sockaddr *)&serv_addr,
		 sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	const int connection_backlog = 5;
	if (listen(server->fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	server->client_addr_len = sizeof(server->client_addr);

	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	return 0;
}

int fs_http_server_run_loop(HttpServerHandle server)
{
	int socket_fd = 0;
	while (1) {
		socket_fd = accept(server->fd,
				   (struct sockaddr *)&server->client_addr,
				   (socklen_t *)&server->client_addr_len);
		if (socket_fd == -1) {
			printf("Accept failed: %s \n", strerror(errno));
			return 1;
		}

		if (!fork()) {
			close(server->fd);

			char *buffer =
				malloc((HTTP_BUFFER_SIZE + 1) * sizeof(char));
			int should_close = 0;
			ssize_t bytes_read =
				recv(socket_fd, buffer, HTTP_BUFFER_SIZE, 0);
			while (should_close == 0 && bytes_read > 0) {
				buffer[bytes_read] = 0;

				// Request parsing
				HttpRequestHandle request =
					fs_http_request_from_buffer(
						bfromcstr(buffer));
				if (request == NULL) {
					// send_bad_request_message(socket_fd, should_close);
					break;
				}

				bstring connection =
					Hashmap_get(request->headers,
						    bfromStatic("Connection"));
				if (connection) {
					should_close =
						biseq(connection,
						      bfromStatic("close"));
				}

				for (int i = 0;
				     i <
				     DArray_count(
					     server->app->request_middlewares);
				     i++) {
					RequestMiddlewareHandler handler = DArray_get(
						server->app->request_middlewares,
						i);
					request = handler(request);
				}

				Hashmap *params = Hashmap_create(NULL, NULL);
				RouteHandler route_handler = match_route(
					server->app->routes,
					request->request_target, &params);
				if (route_handler == NULL) {
					// send not_found
					break;
				}

				HttpResponseHandle response =
					route_handler(request, params);

				for (int i = 0;
				     i <
				     DArray_count(
					     server->app->response_middlewares);
				     i++) {
					ResponseMiddlewareHandler handler = DArray_get(
						server->app
							->response_middlewares,
						i);
					response = handler(response);
				}

				if (params) {
					Hashmap_destroy(params);
				}

				fs_http_request_delete(request);

				if (response) {
					unsigned char *response_buffer =
						fs_http_response_to_buffer(
							response);

					log_send(socket_fd, response_buffer,
						 fs_http_response_size(
							 response));

					fs_http_response_delete(response);
					free(response_buffer);
				}

				if (!should_close) {
					bytes_read = recv(socket_fd, buffer,
							  HTTP_BUFFER_SIZE, 0);
				}
			}

			if (buffer) {
				free(buffer);
			}

			close(socket_fd);
			exit(0);
		}

		close(socket_fd);
	}
}
