#ifndef _RESPONSE_H
#define _RESPONSE_H

#include "bstrlib.h"

#include <stddef.h>
#include <stdint.h>

enum http_status : int16_t {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_BAD_REQUEST = 400,
	HTTP_NOT_FOUND = 404,
	HTTP_INTERNAL_SERVER_ERROR = 500,
};

typedef struct HttpResponse *HttpResponseHandle;

size_t fs_http_response_size(HttpResponseHandle response);
unsigned char *fs_http_response_to_buffer(HttpResponseHandle response);
void fs_http_response_delete(HttpResponseHandle response);

enum http_response_options : int8_t {
	HTTP_VERSION,
	HEADERS,
	BODY,
	COMPRESSION_GZIP,
	CLOSE,
};

typedef struct HttpResponseBuilder *HttpResponseBuilderHandle;

HttpResponseBuilderHandle
fs_http_response_builder_new(enum http_status status_code);
void fs_destroy_http_response_builder(HttpResponseBuilderHandle builder);
void fs_http_response_builder_option(HttpResponseBuilderHandle builder,
				     int option, ...);
void fs_http_response_builder_plain_message(HttpResponseBuilderHandle builder,
					    bstring message);
HttpResponseHandle
fs_http_response_builder_construct(HttpResponseBuilderHandle builder);

#endif
