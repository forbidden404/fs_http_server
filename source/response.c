#include "fs_http_server/response.h"

#include "fs_http_server/dbg.h"
#include <stdlib.h>

struct HttpResponse {
	size_t size;
};

size_t fs_http_response_size(HttpResponseHandle response)
{
	return response->size;
}

unsigned char *fs_http_response_to_buffer(HttpResponseHandle response)
{
	return (unsigned char *)response;
}

void fs_http_response_delete(HttpResponseHandle response)
{
	response->size = 0;
};

struct HttpResponseBuilder {
	enum http_status status_code;
};

HttpResponseBuilderHandle
fs_http_response_builder_new(enum http_status status_code)
{
	HttpResponseBuilderHandle handle = calloc(1, sizeof(*handle));
	check_mem(handle);

	handle->status_code = status_code;

	return handle;

error:
	return NULL;
}

void fs_destroy_http_response_builder(HttpResponseBuilderHandle builder)
{
	free(builder);
}

void fs_http_response_builder_option(HttpResponseBuilderHandle builder,
				     int option, ...)
{
	check_mem(builder);
	if (option < 0) {
		return;
	}

error:
	return;
}

void fs_http_response_builder_plain_message(HttpResponseBuilderHandle builder,
					    bstring message)
{
	check_mem(builder);
	check_mem(message);

error:
	return;
}

HttpResponseHandle
fs_http_response_builder_construct(HttpResponseBuilderHandle builder)
{
	check_mem(builder);

	HttpResponseHandle handle = calloc(1, sizeof(*handle));
	check_mem(handle);

	return handle;

error:
	return NULL;
}
