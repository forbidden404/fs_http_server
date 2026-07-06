#include "../../include/fs_http_server/fs_http_server.h"
#include <stdlib.h>

int main()
{
	HttpAppHandle app = fs_http_app_new();
	HttpServerHandle server = fs_http_server_new(app);

	if (fs_http_server_bind(server, bfromStatic("localhost"), 4221) != 1) {
		goto error;
	}

	fs_http_server_run_loop(server);

	fs_http_server_delete(server);
	server = NULL;
	fs_http_app_delete(app);
	app = NULL;

	return EXIT_SUCCESS;

error:

	if (server) {
		fs_http_server_delete(server);
	}

	if (app) {
		fs_http_app_delete(app);
	}

	return EXIT_FAILURE;
}
