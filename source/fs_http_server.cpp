#include <string>

#include "fs_http_server/fs_http_server.hpp"

exported_class::exported_class()
    : m_name {"fs_http_server"}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
