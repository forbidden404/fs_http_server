#ifndef _FS_HTTP_SERVER_H
#define _FS_HTTP_SERVER_H

#include "bstrlib.h"
#include "hashmap.h"
#include "request.h"
#include "response.h"

// App
typedef struct HttpApp *HttpAppHandle;

HttpAppHandle fs_http_app_new();
void fs_http_app_delete(HttpAppHandle app);

typedef HttpResponseHandle (*RouteHandler)(HttpRequestHandle request,
					   Hashmap *params);
void fs_http_app_register_route(HttpAppHandle app, bstring pattern,
				RouteHandler handler);

typedef HttpRequestHandle (*RequestMiddlewareHandler)(HttpRequestHandle request);
typedef HttpResponseHandle (*ResponseMiddlewareHandler)(
	HttpResponseHandle response);

void fs_http_app_register_request_middleware(HttpAppHandle app,
					     RequestMiddlewareHandler handler);
void fs_http_app_register_response_middleware(HttpAppHandle app,
					      RequestMiddlewareHandler handler);

void fs_http_app_set_directory(HttpAppHandle app, bstring directory);

// Server
typedef struct HttpServer *HttpServerHandle;

HttpServerHandle fs_http_server_new(HttpAppHandle app);
void fs_http_server_delete(HttpServerHandle server);

int fs_http_server_bind(HttpServerHandle server, bstring address, int port);
int fs_http_server_run_loop(HttpServerHandle server);

#endif
