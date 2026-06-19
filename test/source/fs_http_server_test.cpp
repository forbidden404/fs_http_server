#include <string>

#include "fs_http_server/fs_http_server.hpp"

auto main() -> int
{
  auto const exported = exported_class {};

  return std::string("fs_http_server") == exported.name() ? 0 : 1;
}
