#ifndef _ROUTE_MATCHING_H
#define _ROUTE_MATCHING_H

#include "fs_http_server.h"

RouteHandler match_route(Hashmap *routes, bstring target, Hashmap **params);

#endif
