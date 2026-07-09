#include "fs_http_server/route_matching.h"

RouteHandler match_route(Hashmap *routes, bstring target, Hashmap **params)
{
	if (!routes) {
		return NULL;
	}

	if (!target) {
		return NULL;
	}

	if (!params) {
		return NULL;
	}

	return NULL;
}
