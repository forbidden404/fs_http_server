#include "fs_http_server/request.h"

#include "fs_http_server/bstrlib.h"
#include "fs_http_server/hashmap.h"

struct HttpRequest {
	bstring method;
	bstring request_target;
	bstring http_version;

	Hashmap *headers;

	void *data;
};

HttpRequestHandle fs_http_request_from_buffer(bstring buffer)
{
	check_mem(buffer);
	if (blength(buffer) == 0) {
		goto error;
	}

	HttpRequestHandle request = calloc(1, sizeof(*request));
	check_mem(request);

	struct bstrList *tokens = bsplits(buffer, bfromStatic(" \r"));
	for (int i = 0; i < tokens->qty; i++) {
		printf("tokens[%d]: %s\n", i, bdata(tokens->entry[i]));
	}

	return request;

error:

	return NULL;
}
