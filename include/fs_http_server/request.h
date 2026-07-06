#ifndef _REQUEST_H
#define _REQUEST_H

#include "bstrlib.h"
#include "hashmap.h"

typedef struct HttpRequest *HttpRequestHandle;
struct HttpRequest {
	bstring method;
	bstring request_target;
	bstring http_version;

	Hashmap *headers;

	void *data;
};

HttpRequestHandle fs_http_request_from_buffer(bstring buffer);
void fs_http_request_delete(HttpRequestHandle request);

#endif
